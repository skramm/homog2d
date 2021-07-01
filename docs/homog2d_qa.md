# Questions & Answers

###

- Q: what is the difference between the types `Homog2d` and `Epipmat`?
- A: the two hold a 3x3 homogeneous matrix. The difference is on the usage and the default values:
With `Homog2d`, the default is a unit transformation, i.e. an "eye" matrix (null everywhere except the diagonal with ones).
With `Epipmat`, the default is a standard horizontally aligned epipolar geometry:
```
0  0  0
0  0  1
0  1  0
```

- Q: Is this library usable on a Windows platform?
- A: I don't see any reason why that would not be the case, as long as you are able to handle the inclusion of a file in your build process.
However, all the additional stuff here (Opencv graphical demo, test files, ...) will probably not build "out of the box", and will require some build tweaking.


