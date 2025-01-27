# Usage

Before running the code, install libpng with:

```
sudo apt install libpng-dev # parse .png file
```

Verify the installation by:

```
boying@boying-BOHL-WXX9:~$ pkg-config --modversion libpng
1.6.37
```

To run the code, use:

```
make run
```

To delete the excutable, use:

```
make clean
```

### Output:

```
input target dimension k:
250
Image successfully saved to processed.png
```

### Deliverables

- An executable program `pca`
- A readme file `README.md`
- Source code `main.cpp` and Makefile
- A file `processed.png` on using the PCA to perform dimensionality reduction.

### Results

with k = 250:

![](src/processed.png)
