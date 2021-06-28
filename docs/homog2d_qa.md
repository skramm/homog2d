
###

- Q: what is the difference between the types `Homog2d` and `Epipmat`?
- A: the two hold a 3x3 homogeneous matrix. The difference is on the usage and the default values:
With `Homog2d`, the default is a unit transformation, i.e. an "eye" matrix (null everywhere except the diagonal with ones).
With `Epipmat`, the default is a standard horizontaly aligned epipolar geometry:
```
0  0  0
0  0  1
0  1  0
```


