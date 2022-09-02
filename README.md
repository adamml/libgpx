# libgpx

A minimalist, ANSI compliant C library to parse the content of GPX files.

[Background](#background) | [Installation](#installation) | [API](#api) | [Example Usage](#example-usage) | [License](#license)

## Table of Contents

1. [Background](#background)
1. [Installation](#installation)
    - [Dependencies](#dependencies)
1. [API](#api)
    - [Constants](#constants)
        - [LIBGPX__COLLECTION_TYPE_RTE](#libgpxcollectiontyperte)
        - [LIBGPX__COLLECTION_TYPE_TRK](#libgpxcollectiontypetrk)
        - [LIBGPX__COLLECTION_TYPE_TRKSEG](#libgpxcollectiontypetrkseg)
        - [LIBGPX__XML_STRING_ALLOCATED_LENGTH](#libgpxxmlstringallocatedlength)
    - [Types](#types)
        - [libgpx_BoundingBox]
        - [libgpx_GPX](#libgpxgpx)
        - [libgpx_GPXAuthor](#libgpxgpxauthor)
        - [libgpx_GPXCopyright](#libgpxgpxcopyright)
        - [libgpx_GPXEmail](#libgpxgpxemail)
        - [libgpx_GPXLink](#libgpxgpxlink)
        - [libgpx_GPXMetadata](#libgpxgpxmetadata)
        - [libgpx_GPXPoint](#libgpxgpxpoint)
        - [libgpx_GPXPointCollection](#libgpxgpxpointcollection)
    - [Functions](#functions)
        - [int libgpx_countgpxtypes(char *gpx, libgpx_GPXTypeCounts *n_gpx_types)](#int-libgpxcountgpxtypeschar-gpx-libgpxgpxtypecounts-ngpxtypes)
        - [void libgpx_parsegpx(unsigned char *gpx, libgpx_GPX, *sgpx)](#void-libgpxparsegpxunsigned-char-gpx-libgpxgpx-sgpx)
1. [Example Usage](#example-usage)
    - [Parsing](#parsing)
1. [License](#license)

## Background

This C library is designed to parse GPX flavour XML files produced by GPS devices for basic data.
Its API is inspired by the minimalist C library philosophy of [Christopher Wellons](https://github.com/skeeto).
For more detail on that philosophy, see [here](https://nullprogram.com/blog/2018/06/10/).

The header declares `extern "C"`, so should also be usable in C++ code.

## Installation

Copy [`libgpx.h`](./src/libgpx.h) and [`libgpx.c`](./src/libgpx.c) into your include path. Then add 

```c
#include <gpx_reader.h>
```

to your source code and link `libgpx.c` to your compile step, e.g.

```console
gcc .\src\application.c .\src\libgpx.c -I.\src\
```

The `-ansi` flag can be added if compiling with GCC as the code is ANSI compliant.

### Dependencies

`libxml2` is required, and required to be on the include path such the following include statements are recognised:

```c
#include <libxml/tree.h>
#include <libxml/parser.h>
```

When compiling, `libxml2` will need to be linked, e.g.

```console
gcc .\src.sample.c .\src\libgpx.c -I.\src\ -I\lib\libxml\include\ -L\lib\libxml\bin\ -lxml2
```

Internally, `libgpx` has no further dependencies - it does not even include `<stdlib.h>`.

## API

### Constants

#### LIBGPX__COLLECTION_TYPE_RTE

- ***Value:*** 0
- Used to denote a collection of points in a <rte></rte> element

#### LIBGPX__COLLECTION_TYPE_TRK

- ***Value:*** 1
- Used to denote a collection of track segments grouped in a <trk></trk> element

#### LIBGPX__COLLECTION_TYPE_TRKSEG

- ***Value:*** 2
- Used to denote a collection of points in a <trkseg></trkseg> element

#### LIBGPX__XML_STRING_ALLOCATED_LENGTH

- ***Value:*** 65
- Used to allocate memory to character arrays storing strings from the XML file

### Types

#### libgpx_BoundingBox

#### libgpx_GPX

This is the base type, pulling together all the GPX sub-types

- **collections** _[`libgpx_GPXPointCollection`](#libgpxgpxpointcollection) *_ - A pointer to an array of [`libgpx_GPXPointCollection`](#libgpxgpxpointcollection)s, used to group the `<trk></trk>`, `<trkseg><trkseg>` and `<rte></rte>` elements of a GPX file
- **links** _[`libgpx_GPXLink`](#libgpxgpxlink) *_ - A pointer to an array of [`libgpx_GPXLink`](#libgpxgpxlink)s, used to group any links found in a GPX file and connect them with their parent element
- **metadata** _[`libgpx_GPXMetadata`](#libgpxgpxmetadata)_ - Contains detailed metadata about the GPX file
- **n_GPXLink** _s]ize_t_ - The length of the array `links` points to
- **n_GPXPoint** _size_t_ - length of the array `points` points to
- **n_GPXPointCollection** _size_t_ - The length of the array `collections`
- **points** _[`libgpx_GPXPoint`](#libgpxgpxpoint) *_  - A pointer to an array of [`lib_GPXPoint`](#libgpxgpxpoint)s, used to group together the contents of the `<rtept></rtept>`, `<trkpt></trkpt>` and `<wpt></wpt>` elements of the GPX file

#### libgpx_GPXAuthor

Contains information about the author of the GPX file for [`libgpx_GPXMetadata`](#libgpxgpxmetadata) types

- **email** _[`libgpx_GPXEmail`](#libgpxgpxemail)_ - A contact e-mail address for the GPX author
- **name**[[`LIBGPX__XML_STRING_ALLOCATED_LENGTH`](#libgpxxmlstringallocatedlength)] - The name of the author of the GPX file

#### libgpx_GPXCopyright

Contains copyright information about the GPX file for [`libgpx_GPXMetadata`](#libgpxgpxmetadata) types

#### libgpx_GPXEmail

Contains e-mail contact information for the author of the GPX file for [`libgpx_GPXAuthor`] types

#### libgpx_GPXLink

Contains information related to external links from the GPX file.

- **href**[[`LIBGPX__XML_STRING_ALLOCATED_LENGTH`](#libgpxxmlstringallocatedlength)] _char_ - The URI the link points to
- **member_of** _int_ - Indicates the ID of the node ([`libgpx_GPXAuthor`](#libgpxgpxauthor), [`libgpx_GPXMetadata`](#libgpxgpxmetadata), [`libgpx_GPXPoint`](#libgpxgpxpoint) or [`libgpx_GPXPointCollection`](#libgpxpointcollection)). If the node is [`libgpx_GPXAuthor`](#libgpxgpxauthor) the ID is -2, if the node is [`libgpx_GPXMetadata`](#libgpxgpxmetadata) the value is -1, otherwise the node ID reflects the value in the lists of [`libgpx_GPXPoint`](#libgpxgpxpoint) or [`libgpx_GPXPointCollection`](#libgpxgpxpointcollection) held in [`libgpx_GPX`](#libgpxgpx)
- **text**[[`LIBGPX__XML_STRING_ALLOCATED_LENGTH`](#libgpxxmlstringallocatedlength)] _char_
- **type**[[`LIBGPX__XML_STRING_ALLOCATED_LENGTH`](#libgpxxmlstringallocatedlength)] _char_

#### libgpx_GPXMetadata

Contains detailed metadata about the GPX file. [`libgpx_GPXAuthor`](#libgpxgpxauthor) and [`libgpx_Copyright`](#libgpxgpxcopyright) types are nested in this type.

#### libgpx_GPXPoint

Contains information about the latitude, longitude and elevation points in a GPX file, and how fixes were obtained. These points may be tracks or planned routes or waypoints.

#### libgpx_GPXPointCollection

Contains informtion grouping instances of [`libgpx_GPXPoint`](#libgpxgpxpoint) into routes, tracks or track segments.

- **collection_type**:2 _unsigned int_ - One of [LIBGPX__COLLECTION_TYPE_RTE](#libgpxcollectiontyperte), [LIBGPX__COLLECTION_TYPE_TRK](#libgpxcollectiontypetrk) or [LIBGPX__COLLECTION_TYPE_TRKSEG](#libgpxcollectiontypetrkseg)
- **id** *size_t* - A sequential id shared across [`libgpx_GPXPoint`](#libgpxgpxpoint) and [`libgpx_GPXPointCollection`](#libgpxgpxpointcollection)
- **member_of** _int_ - Allows nesting of [`libgpx_GPXPointCollection`](#libgpxgpxpointcollection)s, for example track segments inside tracks. Points to `id` of another [`libgpx_GPXPointCollection`](#libgpxgpxpointcollection) or is -1 if the [`libgpx_GPXPointCollection`](#libgpxgpxpointcollection) is a child of the root `<gpx></gpx>` element
- **name**[[`LIBGPX__XML_STRING_ALLOCATED_LENGTH`](#libgpxxmlstringallocatedlength)] _char_ -
- **type**[[`LIBGPX__XML_STRING_ALLOCATED_LENGTH`](#libgpxxmlstringallocatedlength)] _char_ -

### Functions

#### void libgpx_bounding_box_from_gpx_object(libgpx_GPX* sgpx, libgpx_BoundingBox* box)

#### int libgpx_count_gpx_types(char* gpx, libgpx_GPX* sgpx)

- ***Inputs***
    - **gpx** _char *_
    - ***sgpx** _libgpx_GPX *_
- ***Returns***
    - An `int`, 1 on success, -1 on failure

#### void libgpx_parse_gpx(unsigned char* gpx, libgpx_GPX* sgpx)

Parses a GPX file from a charcter array into a libgpx_GPX structure.

- ***Inputs***
    - **gpx** _unsigned char *_
    - **sgpx** _libgpx_GPX *_

## Example Usage

### Parsing

There are a couple of variables to initialise at the start of the code _(These example use malloc from `<stdlib.h>`, but this can be easily substituted in applications which are not linked against the standard library)._:

```c
libgpx_GPX *gpx = malloc(sizeof(libgpx_GPX));
if(gpx != NULL){
    libgpx_GPXTypeCounts *ngpx = malloc(sizeof(libgpx_GPXTypeCounts));

        /** Drop-in following example code here */
        
}
```

Once the XML document loaded to a char array pointer (in the example below, named `buffer`), then the first step is to count the number of point collections, points and links in the XML file:

```c
char *buffer = 0; /** wchar_t is a better option for some XML files */

/** Read your GPX data as XML into buffer here and then... */

int status_ngpx = libgpx_count_gpx_types(buffer, ngpx);
```

`libgpx_count_gpx_types` will return a negative integer if it encountered an error, so that can be checked as the code progresses to allocating the arrays to store the various GPX data types.

```c
if(status_ngpx > 0){
    /** Allocate memory to the storage arrays*/
    libgpx_GPXPoint *points = malloc(sizeof(libgpx_GPXPoint) * ngpx->n_points);
    if(points != NULL){
        libgpx_GPXLink *links = malloc(sizeof(libgpx_GPXLink) * ngpx->n_links);
        if(links != NULL){
            libgpx_GPXPointCollection *collections = malloc(sizeof(libgpx_GPXPointCollection) * ngpx->n_point_collections);

            /** Move on to parsing the XML here */

        }
    }
}
```
The XML can  then be parse with this command:

```c
libgpx_parse_gpx((unsigned char*)buffer, gpx);
```

Putting it all together...

```c
#include <libgpx.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

wchar_t *buffer = 0;
long len;
libgpx_GPX *gpx = malloc(sizeof(libgpx_GPX));
if(gpx){

    /** Read the file contenst to `buffer` here */

    if (buffer){
        int gpx_type_count_status = libgpx_count_gpx_types((char *)buffer, gpx);

        if(gpx_type_count_status > 0){
            libgpx_GPXPoint *points = malloc(sizeof(libgpx_GPXPoint) * gpx->n_GPXPoint);
            if(points){
                libgpx_GPXLink *links = malloc(sizeof(libgpx_GPXLink) * gpx->n_GPXLink);
                if(links){
                    libgpx_GPXPointCollection *collections = malloc(sizeof(libgpx_GPXPointCollection) * gpx->n_GPXPointCollection);
                    if(collections){
                        gpx->points = points;
                        gpx->collections = collections;
                        gpx->links = links;

                        libgpx_parse_gpx((unsigned char*)buffer, gpx);
                    }
                }
            }   
        }
    }
return 0;
}
```

## License

This software is "licensed" with the [Unlicense](./UNLICENSE). As such, this is free and unencumbered software released into the public domain.