# CS 184: Computer Graphics and Imaging, Spring 2023

## Project 1: Rasterizer
## Michael Lin, Rachel Lee
 *** 
### Section I: Drawing Single-Color Triangles
- **Walk through how you rasterize triangles in your own words.**
    - We first find the smallest possible bounding box of the triangle using the three provided vertices and finding the minimum and maximum points of the bounding box. Then, we iterate through each sample pixel within the bounding box and use the point-in-triangle test to check if the sample point is within the triangle. If at any instance the sample point fails the three line tests, then we break from the loop and check again from the next column.
- **Explain how your algorithm is no worse than one that checks each sample within the bounding box of the triangle.**
    - Since the algorithm iterates through the x and y values of the bounding box maximum, this ensures that in the worst possible case that the maximum possible iterations in the double for-loop is O(xy) which is the area of the bounding box of the triangle. The bounding box of the triangle is also dependent on the maximum largest possible x and y value of the triangle, so the maximum possible number of sample points checked is within the bounding box.
- **Show a png screenshot of basic/test4.svg with the default viewing parameters and with the pixel inspector centered on an interesting part of the scene.**\
![test4](./images/img1.png)
- **Extra credit: Explain any special optimizations you did beyond simple bounding box triangle rasterization, with a timing comparison table (we suggest using the c++ clock() function around the svg.draw() command in DrawRend::redraw() to compare millisecond timings with your various optimizations off and on).**
    - To optimize the filling of the pixels while iterating through each sample point in each column of the bounding box, we break from the inner for loop if at any point there is a sample point that fails the point-in-triangle test. This allows for a noticeable improvement in runtime as we reduce wasteful computation of the vector cross products and can move on to the next column in the outer loop quickly.


| Before optimization | After optimization |
| ----------- | ----------- |
| 0.00105 seconds | 0.000843 seconds |
| 0.001273 seconds | 0.000792 seconds |
| 0.00109 seconds | 0.000884 seconds |
| 0.001151 seconds | 0.000791 seconds |
