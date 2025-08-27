#include <iostream>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

// functions for the memory management

// create a 2d array height = rows and width = cols
int **create2dArray(int height, int width)
{
    int **arr = new int *[height];
    for (int i = 0; i < height; ++i)
        arr[i] = new int[width];
    return arr;
}

// memory deallocation for the 2d array
void delete2darr(int **arr, int height)
{
    for (int i = 0; i < height; ++i)
        delete[] arr[i];
    delete[] arr;
}

// now to allocate a 3d array dynamically to store the image data height * width * channels
int ***make3darr(int height, int width, int channels)
{
    int ***arr = new int **[height];
    for (int i = 0; i < height; ++i)
    {
        arr[i] = new int *[width];
        for (int j = 0; j < width; ++j)
            arr[i][j] = new int[channels];
    }
    return arr;
}

// now deallocates the memory occupied by the 3d array
void delete3darr(int ***arr, int height, int width)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
            delete[] arr[i][j];
        delete[] arr[i];
    }
    delete[] arr;
}
// for horizontal seam removal we will optimize our code by 1st transpose the img then apply the verical seam removal technique
int ***transposeimage(int ***imageinfo, int height, int width)
{
    int newheight = width;

    int newwidth = height;
    int ***transposedImage = make3darr(newheight, newwidth, 3);

    for (int r = 0; r < newheight; ++r)
    {
        for (int c = 0; c < newwidth; ++c)
        {
            transposedImage[r][c][0] = imageinfo[c][r][0];
            transposedImage[r][c][1] = imageinfo[c][r][1];
            transposedImage[r][c][2] = imageinfo[c][r][2];
        }
    }
    return transposedImage;
}

// now we will calculate the energy value of each pixel  of the input image in a 2d matrix

int **calculateEnergy(int ***imageinfo, int height, int width)
{
    int **energymapping = create2dArray(height, width);
    for (int r = 0; r < height; ++r)
    {
        for (int c = 0; c < width; ++c)
        {
            // pixel at the borders are given a high value so that they will stay in the image
            if (r == 0 || r == height - 1 || c == 0 || c == width - 1)
            {
                energymapping[r][c] = 1e9;
            }
            // now gradient calculation will be done for the pixels in the non_border area
            // first we have to get the colors of the neighbouring pixels
            else
            {
                int *left = imageinfo[r][c - 1];
                int *right = imageinfo[r][c + 1];
                int *up = imageinfo[r - 1][c];
                int *down = imageinfo[r + 1][c];
                // now we will calculate the energy value using the formula
                int deltaREDx = right[0] - left[0];
                int deltaGreenx = right[1] - left[1];
                int deltaBluex = right[2] - left[2];
                int energyinX = deltaREDx * deltaREDx + deltaGreenx * deltaGreenx + deltaBluex * deltaBluex;

                int deltaREDy = down[0] - up[0];
                int deltaGreeny = down[1] - up[1];
                int deltaBluey = down[2] - up[2];
                int energyinY = deltaREDy * deltaREDy + deltaGreeny * deltaGreeny + deltaBluey * deltaBluey;
                energymapping[r][c] = energyinX + energyinY; // final energy value of the pixel
            }
        }
    }
    return energymapping; // return the energy mapping
}

// no we will find the vertical seam to be removed from the image
int *findtheverticalSeam(int **energymapping, int height, int width)
{
    int **cumiltativeenergymap = create2dArray(height, width);
    // first row is same as the energy mapping array
    for (int c = 0; c < width; ++c)
        cumiltativeenergymap[0][c] = energymapping[0][c];

    // now we will fill the cumulative energy map for the rest of the pixels

    for (int r = 1; r < height; ++r)
    {
        for (int c = 0; c < width; ++c)
        {
            int minparenttopenergy = cumiltativeenergymap[r - 1][c]; // parent energy from the top
            if (c > 0)                                               // if not the first column, check the left parent
                minparenttopenergy = min(minparenttopenergy, cumiltativeenergymap[r - 1][c - 1]);
            if (c < width - 1) // if not the last column, check the right parent
                minparenttopenergy = min(minparenttopenergy, cumiltativeenergymap[r - 1][c + 1]);
            cumiltativeenergymap[r][c] = energymapping[r][c] + minparenttopenergy; // add the energy of the current pixel +  the minimum parent energy
        }
    }

    // now we will find the minimum energy in the last row
    int minenerygy = INT_MAX;
    int endcol = -1;
    for (int c = 0; c < width; ++c)
    {
        if (cumiltativeenergymap[height - 1][c] < minenerygy)
        {
            minenerygy = cumiltativeenergymap[height - 1][c];
            endcol = c; // store the column index of the minimum energy
        }
    }

    // now we will backtrack to find the mon energy path i.e the vertical seam to be removed

    int *verticalseampath = new int[height];
    verticalseampath[height - 1] = endcol;
    for (int r = height - 2; r >= 0; --r)
    {
        int prevcol = verticalseampath[r + 1]; // get the column index of the next row
        int currcol = prevcol;
        int minenergy = cumiltativeenergymap[r][prevcol]; // initialize the minimum energy to the current column
        // check the left and right columns for backtracking
        if (prevcol > 0 && cumiltativeenergymap[r][prevcol - 1] < minenergy)
        {
            minenergy = cumiltativeenergymap[r][prevcol - 1];
            currcol = prevcol - 1; // move to the left column
        }
        if (prevcol < width - 1 && cumiltativeenergymap[r][prevcol + 1] < minenergy)
        {
            minenergy = cumiltativeenergymap[r][prevcol + 1];
            currcol = prevcol + 1; // move to the right column
        }
        verticalseampath[r] = currcol; // store the column index of the current
    }
    delete2darr(cumiltativeenergymap, height); // free the memory occupied by the cumulative energy map
    return verticalseampath;                   // return the vertical seam path
}

// now to remove the vertical seam from the image
void removeverticalseamlowenergy(int ***&imageinfo, int *vertiseampath, int height, int &width)
{
    int newwidth = width - 1;                             // new width after removing a seam
    int ***newimageinfo = make3darr(height, newwidth, 3); // create a new image array with reduced width
    for (int r = 0; r < height; ++r)
    {
        int coltoremove = vertiseampath[r]; // get the column index to remove
        int newcol = 0;
        for (int oldcolumn = 0; oldcolumn < width; ++oldcolumn)
        {
            if (oldcolumn == coltoremove) // if the column is the one to remove, skip it
                continue;
            // copy the pixel data to the new image array
            newimageinfo[r][newcol][0] = imageinfo[r][oldcolumn][0]; // red
            newimageinfo[r][newcol][1] = imageinfo[r][oldcolumn][1]; // green
            newimageinfo[r][newcol][2] = imageinfo[r][oldcolumn][2]; // blue
            ++newcol;                                                // increment the new column index
        }
    }
    delete3darr(imageinfo, height, width); // free the memory occupied by the old image array
    imageinfo = newimageinfo;              // update the image array to the new one
    width = newwidth;                      // return the new image array
}

int main()
{
    string inputpath, outputpath;
    int newwidth, newheight;
    cout << "Enter the input image path: ";
    cin >> inputpath;
    cout << "Enter the output image path: ";
    cin >> outputpath;
    cout << "Enter the new width and height: ";
    cin >> newwidth >> newheight;

    cv::Mat image = cv::imread(inputpath, cv::IMREAD_COLOR); // read the input image
    if (image.empty())
    {
        cerr << "Error: Could not open /find the image!" << endl;
        return -1;
    }

    int orginalheight = image.rows; // get the original height of the image
    int orginalwidth = image.cols;  // get the original width of the image

    if (newwidth > orginalwidth || newheight > orginalheight)
    {
        cerr << " New width and height should be less than the original width and height!" << endl;
        return -1;
    }

    int ***imageinfo = make3darr(orginalheight, orginalwidth, 3); // create a 3d array to store the image data
    for (int r = 0; r < orginalheight; ++r)
    {
        for (int c = 0; c < orginalwidth; ++c)
        {
            imageinfo[r][c][0] = image.at<cv::Vec3b>(r, c)[2]; // red
            imageinfo[r][c][1] = image.at<cv::Vec3b>(r, c)[1]; // green
            imageinfo[r][c][2] = image.at<cv::Vec3b>(r, c)[0]; // blue
        }
    }

    int currentheight = orginalheight; // current height of the image
    int currentwidth = orginalwidth;   // current width of the image
    // now we will remove the vertical seams until the width is equal to the new width

    int seamstoremove = orginalwidth - newwidth; // number of seams to remove
    if (seamstoremove > 0)
        cout << "\nRemoving " << seamstoremove << " vertical seams..." << endl;

    for (int i = 0; i < seamstoremove; ++i)
    {
        int **energymapping = calculateEnergy(imageinfo, currentheight, currentwidth);
        int *vertiseampath = findtheverticalSeam(energymapping, currentheight, currentwidth); // find the vertical seam to remove
        removeverticalseamlowenergy(imageinfo, vertiseampath, currentheight, currentwidth);   // remove the vertical seam
        delete2darr(energymapping, currentheight);                                            // free the memory occupied by the energy mapping
        delete[] vertiseampath;                                                               // free the memory occupied by the vertical seam path
        // currentwidth--;                                                                       // decrement the current width
        cout << "Removed vertical seam " << i + 1 << " of " << seamstoremove << endl; // print the progress
    }
    // part2 horizontal seam removal
    int seamstoremovehorizontal = orginalheight - newheight; // number of seams to remove
    if (seamstoremovehorizontal > 0)
    {
        cout << "\nRemoving " << seamstoremovehorizontal << " horizontal seams..." << endl;

        // first we will transpose the image
        int ***transposed_img = transposeimage(imageinfo, currentheight, currentwidth);
        delete3darr(imageinfo, currentheight, currentwidth); // free the memory occupied by the old image array
        imageinfo = transposed_img;                          // update the image array to the transposed one
        int transposed_height = currentwidth;
        int transposed_width = currentheight;

        // now run the vertical seam removal on the transposed image
        for (int i = 0; i < seamstoremovehorizontal; ++i)
        {
            int **energymapping = calculateEnergy(imageinfo, transposed_height, transposed_width);
            int *horizontalseampath = findtheverticalSeam(energymapping, transposed_height, transposed_width);
            removeverticalseamlowenergy(imageinfo, horizontalseampath, transposed_height, transposed_width); // remove the vertical seam
            delete2darr(energymapping, transposed_height);                                                   // free the memory occupied by the energy mapping
            delete[] horizontalseampath;                                                                     // free the memory occupied by the vertical seam path
            cout << "Removed horizontal seam " << i + 1 << " of " << seamstoremovehorizontal << endl;        // print the progress
        }
        // now we will transpose the image back to its original format
        int ***final_img = transposeimage(imageinfo, transposed_height, transposed_width);
        delete3darr(imageinfo, transposed_height, transposed_width); 

        imageinfo = final_img;
        currentheight = transposed_width;
        currentwidth = transposed_height; 
    }

    // now we will bridge back the 3darray to a Mat object
    cv::Mat outputimage(currentheight, currentwidth, CV_8UC3); // create a new Mat object with the new width and height
    for (int r = 0; r < currentheight; ++r)
    {
        for (int c = 0; c < currentwidth; ++c)
        {
            outputimage.at<cv::Vec3b>(r, c) = cv::Vec3b(imageinfo[r][c][2], imageinfo[r][c][1], imageinfo[r][c][0]); // set the pixel values
        }
    }

    // save the output image
    if (cv::imwrite(outputpath, outputimage))
    {
        cout << "we have successfully saved the output image to " << outputpath << endl;
    }
    else
    {
        cerr << "Could not save the output image!" << endl;
        return -1;
    }
    delete3darr(imageinfo, currentheight, currentwidth); // free the memory occupied by the image data
    return 0;                                            // return success
}
