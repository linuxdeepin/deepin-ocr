# deepin-ocr

Deepin  ocr is a .

### Dependencies

### Build dependencies

_The **master** branch is current development branch, build dependencies may changes without update README.md, refer to `./debian/control` for a working build depends list_

* pkg-config
* libdtkwidget-dev
* Qt5(>=5.6) with modules:
  * qt5-default
  * qt5-qmake
  * qtbase5-dev
  
    



## Installation

### Build from source code

1. Make sure you have installed all dependencies.

_Package name may be different between distros, if dde-file-manager is available from your distro, check the packaging script delivered from your distro is a better idea._

Assume you are using [Deepin](https://distrowatch.com/table.php?distribution=deepin) or other debian-based distro which got dde-file-manager delivered:

``` shell
$ apt build-dep deepin-ocr
```

2. Build:

```
$ cd deepin-album
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

3. Install:

```
$ sudo make install
```

The executable binary file could be found at `/usr/bin/deepin-ocr `

## Usage

Execute `deepin-ocr`

## Documentations

 - [User Documentation](https://wikidev.uniontech.com/index.php?title=%E7%9B%B8%E5%86%8C) | [用户文档](https://wikidev.uniontech.com/index.php?title=%E7%9B%B8%E5%86%8C)

## Getting help

 - [Official Forum](https://bbs.deepin.org/)
 - [Developer Center](https://github.com/linuxdeepin/developer-center)
 - [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
 - [IRC Channel](https://webchat.freenode.net/?channels=deepin)
 - [Wiki](https://wiki.deepin.org/)

## Getting involved

We encourage you to report issues and contribute changes

 - [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en) (English)
 - [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

## License

deepin-ocr is licensed under [GPLv3](