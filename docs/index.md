<h1 align="middle">CS 184: Computer Graphics and Imaging, Spring 2018</h1>
<h2 align="middle">Project 1: Rasterizer</h1>
<h3 align="middle">YOUR NAME, CS184-??</h2>

## Overview

### Part 5: "Pixel sampling" for texture mapping

- Explain pixel sampling in your own words and describe how you implemented it to perform texture mapping. Briefly discuss the two different pixel sampling methods, nearest and bilinear.
  
Pixel sampling is essentially converting discrete pixel information to a continuous 2D-coordinate domain. In texture mapping, for any given point within the triangle in the sample buffer, we will convert its coordinate to the texture domain using the barycentric system. Then we will sample the texture at `(u, v)` using the sampling technique specified.

In the project, we implemented the nearest neighbor and the bilinear sampling method. `P_NEAREST` takes the floating point `(u, v)` and round it to the nearest integer. This effective samples the closest pixel to `(u, v)` and hence the name. `P_LINEAR` on the other hand, takes the weighted average of the four closest pixel value, namely `(floor(u), floor(v))`, `(floor(u), ceil(v))`, `(ceil(u), floor(v))`, `(ceil(u), ceil(v))`.

- Check out the svg files in the `svg/texmap/` directory. Use the pixel inspector to find a good example of where bilinear sampling clearly defeats nearest sampling. Show and compare four png screenshots using nearest sampling at 1 sample per pixel, nearest sampling at 16 samples per pixel, bilinear sampling at 1 sample per pixel, and bilinear sampling at 16 samples per pixel.

<div align="middle">
  <table style="width=100%">
    <tr>
      <td>
        <img src="images/t5-nearest-1.png" align="middle" width="400px"/>
        <figcaption align="middle">Nearest 1</figcaption>
      </td>
      <td>
        <img src="images/t5-linear-1.png" align="middle" width="400px"/>
        <figcaption align="middle">Bilinear 1</figcaption>
      </td>
    </tr>
    <br>
    <tr>
      <td>
        <img src="images/t5-nearest-16.png" align="middle" width="400px"/>
        <figcaption align="middle">Nearest 16</figcaption>
      </td>
      <td>
        <img src="images/t5-linear-16.png" align="middle" width="400px"/>
        <figcaption align="middle">Bilinear 16</figcaption>
      </td>
    </tr>
  </table>
</div>

- Comment on the relative differences. Discuss when there will be a large difference between the two methods and why.

In the example above, the bilinear method produces less jaggies than the nearest neighbor. This effect will be especially prominent if the area in the texture has high frequency signals. In this case, the thin white line is the high frequency signal.

### Part 6: "Level sampling" with mipmaps for texture mapping

- Explain level sampling in your own words and describe how you implemented it for texture mapping.

Level sampling works by combine differently filter texture profile into a single structure. This is because different regions on screen will have different footprint on the texture, if we indiscriminately apply a low pass filter in an attempt to get rid of the jaggies, area will low frequencies to begin with will appear over-blurred.

To implement level sampling, we will first generate and store the levels in the `Texture` class. We will compute which level or levels to use based on the level sampling policy. For this project, we have `L_ZERO`, `L_NEAREST`, and `L_LINEAR`. `L_ZERO` will simply uses the full resolution will applying the texture. `L_NEAREST` will select the closest integer level using the formula:
$$L = \max \Big( \sqrt{\big(\frac{du}{dx}\big)^2 + \big(\frac{dv}{dx}\big)^2},  \sqrt{\big(\frac{du}{dy}\big)^2 + \big(\frac{dv}{dy}\big)^2} \Big)$$ and 
$$D = \log_2 L$$

So essentially,

$$\texttt{L\_NEAREST} = \texttt{round}(D)$$

`L_LINEAR` works similarly to `L_NEAREST`, but instead of selecting a integer level, it linearly interpolate the two closest levels.  This allows us to mix two levels together and produces better antialiased result.

- You can now adjust your sampling technique by selecting pixel sampling, level sampling, or the number of samples per pixel. Describe the tradeoffs between speed, memory usage, and antialiasing power between the three various techniques.

Among all three, supersampling has the greatest impact on performance. Since every pixel needs to be sampled multiple times on each draw command, drawing time will increase significantly.

Level sampling mitigates that by sacrificing space. It pre-computes different sampling level so that we don't need to do it on the fly. Doing so require us to save the MIP levels, thereby increasing the memory footprint.

Finally, bilinear pixel sampling is the fastest of them all, and has similar memory usage compared the default nearest neighbor method, but the antialiasing performance isn't as great as the other two method.

- Using a png file you find yourself, show us four versions of the image, using the combinations of `L_ZERO` and `P_NEAREST`, `L_ZERO` and `P_LINEAR`, `L_NEAREST` and `P_NEAREST`, as well as `L_NEAREST` and `P_LINEAR`.

<div align="middle">
  <table style="width=100%">
    <tr>
      <td>
        <img src="images/t6-zero-nearest.png" align="middle" width="400px"/>
        <figcaption align="middle">L_ZERO + P_NEAREST</figcaption>
      </td>
      <td>
        <img src="images/t6-zero-linear.png" align="middle" width="400px"/>
        <figcaption align="middle">L_ZERO + P_LINEAR</figcaption>
      </td>
    </tr>
    <br>
    <tr>
      <td>
        <img src="images/t6-nearest-nearest.png" align="middle" width="400px"/>
        <figcaption align="middle">L_NEAREST + P_NEAREST</figcaption>
      </td>
      <td>
        <img src="images/t6-nearest-linear.png" align="middle" width="400px"/>
        <figcaption align="middle">L_NEAREST + P_LINEAR</figcaption>
      </td>
    </tr>
  </table>
</div>