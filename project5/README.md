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
report file report.txt successfully generated.
```

### Deliverables

- An executable program `pca`
- A readme file `README.md`
- Source code `main.cpp` and Makefile
- A file `processed.png` on using the PCA to perform dimensionality reduction.

### Results

with k = 250:

![](src/processed.png)

奇怪......为什么会差这么多呢？
