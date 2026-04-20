<p align="center">
    <img src="media/logo.png" alt="jobb" width="200"/>
</p>
  
<p align="center">
  <em>A search utility for TODO statements<br>in codebases.</em>
</p>
  
<p align="center">
    <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="MIT License" />
  <img src="https://img.shields.io/github/last-commit/simon-danielsson/jobb/main?style=flat-square&color=blue" alt="Last commit" />
</p>
  
<p align="center">
  <a href="#info">Info</a> •
  <a href="#install">Install</a> •
  <a href="#usage">Usage</a> •
  <a href="#license">License</a>
</p>  
  
---
<div id="info"></div>

## Info
  
Jobb is a search utility, but don't be mistaken! Jobb cannot be compared to tools such as grep and ripgrep due to it only being optimized for finding TODO statements in large codebases - but its very good at it! Jobb returns a formatted list of all TODO statements found, each with a reference to file and line number. In addition to this, Jobb collects entire TODO paragraphs, not just lines.
  
> [!IMPORTANT]  
> Jobb relies on unix libraries, so there is no support for Windows.
  
---
<div id="install"></div>

## Install
  
Clone and compile on your own machine. This project uses [nob.h](https://github.com/tsoding/nob.h) as its build-system and is not reliant on any third-party tools apart from a C compiler.  
  
``` bash
# clone this repo
git clone https://github.com/simon-danielsson/jobb

# create build directory
mkdir build

# bootstrap nob
cc -o ./nob/nob ./nob/nob.c

# run nob to compile the program
./nob/nob

# run program 
# (make a bash alias for this to avoid dealing with bin paths)
./build/main
```
  
---
<div id="usage"></div>

## Usage
  
``` bash
$ jobb help   : view help
$ jobb <path> : specify a file or directory
```
  
---
<div id="license"></div>

## License
  
This project is licensed under the [MIT License](https://github.com/simon-danielsson/jobb/blob/main/LICENSE).  
 
