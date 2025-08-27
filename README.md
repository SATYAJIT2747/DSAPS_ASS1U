# DSAPS Assignment 1 

**Roll Number:** 2025201055



---

## Question 1: Seam Carving

### Approach 

implement a image resizing technique called Seam Carving.

* **Data Structures:**
    * i have  used a 3D integer array  to store the RGB values of the image (Height × Width × 3).
    * A 2D integer array  keeps track of the energy value for each pixel.
    * Another 2D array  stores the cumulative energy for dynamic programming.
    * A 1D array  holds the column indices of pixels forming the seam with the loist energy.

* **Algorithms:**
    * **Energy Calculation:**  This  is sums the squared differences of colors with horizontal and vertical neighbors. Border pixels are assigned very high energy to avoid removing them.
    * **Seam Identification:** i have used a dynamic programming approach to  find the vertical seam with minimum energy. i build a cumulative energy map from top to bottom, where each cell adds its own energy to the minimum energy of the three parent pixels above. The seam is then traced back starting from the smallest value in the last row.
    * **Seam Removal:** To remove a seam, a new 3D array is created. Pixels are copied from the original image to the new array, skipping the pixels that belong to the identified seam.

* **Optimizations:**
    * Horizontal seam carving is done by transposing the image, applying the vertical seam carving, and then transposing back. 
    * This avoids repeating code.
    * both the transpose method and a separate horizontal algorithm have the same theoretical time complexity of O(H×W). 
  
**cache locality.**
The vertical seam algorithm is cache-friendly because it processes adjacent pixels that are close together in memory. But  a separate horizontal algorithm would be cache-unfriendly, as it would need to access pixels in different rows that are far apart in memory, causing slow "cache misses". 

### Compilation and Execution



1. **Compile:**
    ```bash
    g++ 2025201055_A1_Q1.cpp -o q1_seam_carving `pkg-config --cflags --libs opencv4`
    ```
    

2. **Run:**
    ```bash
    ./q1_seam_carving
    ```
    The program will ask for the input image path, output image path, and the  width and height.

---

## Question 2: Trie Harder

### Approach 

This program implements a spell checker with autocomplete and autocorrect using a Trie data structure.

* **Data Structure:**
    * i have used a Trie to store dictionary words efficiently. Each node has an array of 26 pointers (for 'a'-'z') and a boolean `isTerminal` to mark the end of a word.

* **Algorithms:**
    * **Spell Check:** To check a word, the program traverses the Trie according to its characters. If the traversal ends at a node with `isTerminal = true`, the word exists.
    * **Autocomplete:** For autocomplete, i first reach the Trie node matching the prefix, then do a  (DFS) to collect all terminal words in the subtree.
    * **Autocorrect:** For autocorrect, i find words within a Levenshtein distance of 3 or less. A DFS traversal computes the distance dynamically, cut branches where the distance exceeds the limit 3 which  makes the algorithm fast and memory-efficient.

### Compilation and Execution



1. **Compile:**
    ```bash
    g++ 2025201055_A1_Q2.cpp -o q2_trie_spell_checker
    ```

2. **Run:**
    ```bash
    ./q2_trie_spell_checker
    ```
    

---

## Question 3: Battle of the Banners

### Approach Description

This program is for  silhouette of banners and handles update/print queries with $O(\log N)$ complexity per query by using a segment tree ds.

* **Data Structures:**
    * **Segment Tree:** The main structure is a Segment Tree with lazy Propagation. Each node stores the min and max height of a range. A `lazytag` array is used for efficient updates.
   

* **Algorithms & Optimizations:**
    * **Coordinate Compression:**as  Coordinates can be large but queries are limited . i compress coordinates by sorting unique left and right values and mapping them to smaller indices. The segment tree is built over this smaller range.
    * **Merge Sort:** merge sort is used to sort coordinates for compression.
    * **Segment Tree with Lazy Propagation:**
        * **Updates:** `rangechangeheight` updates a banner’s range. If fully covered, i apply a lazy tag instead of recursing further. Recursion stops if a taller banner already covers the range.
        * **Printing:** `collect` traverses the tree, pushes down lazy updates, and prints key points whenever the height changes along compressed coordinates.

### Compilation and Execution

1. **Compile:**
    ```bash
    g++ 2025201055_A1_Q3.cpp -o q3_banner_silhouette
    ```

2. **Run:**
    ```bash
    ./q3_banner_silhouette
    ```
   