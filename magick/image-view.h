/*
  Copyright 1999-2010 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.
  
  You may not use this file except in compliance with the License.
  obtain a copy of the License at
  
    http://www.imagemagick.org/script/license.php
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITTransferNS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore image view methods.
*/
#ifndef _MAGICKIMAGE_IMAGE_VIEW_H
#define _MAGICKIMAGE_IMAGE_VIEW_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _ImageView
  ImageView;

typedef MagickBooleanType
  (*DuplexTransferImageViewMethod)(const ImageView *,const ImageView *,
    ImageView *,void *),
  (*GetImageViewMethod)(const ImageView *,void *),
  (*SetImageViewMethod)(ImageView *,void *),
  (*TransferImageViewMethod)(const ImageView *,ImageView *,void *),
  (*UpdateImageViewMethod)(ImageView *,void *);

extern MagickExport char
  *GetImageViewException(const ImageView *,ExceptionType *);

extern MagickExport const IndexPacket
  *GetImageViewVirtualIndexes(const ImageView *);

extern MagickExport const PixelPacket
  *GetImageViewVirtualPixels(const ImageView *);

extern MagickExport Image
  *GetImageViewImage(const ImageView *);

extern MagickExport ImageView
  *CloneImageView(const ImageView *),
  *DestroyImageView(ImageView *),
  *NewImageView(Image *),
  *NewImageViewRegion(Image *,const ssize_t,const ssize_t,const size_t,
    const size_t);

extern MagickExport IndexPacket
  *GetImageViewAuthenticIndexes(const ImageView *);

extern MagickExport MagickBooleanType
  DuplexTransferImageViewIterator(ImageView *,ImageView *,ImageView *,
    DuplexTransferImageViewMethod,void *),
  GetImageViewIterator(ImageView *,GetImageViewMethod,void *),
  IsImageView(const ImageView *),
  SetImageViewIterator(ImageView *,SetImageViewMethod,void *),
  TransferImageViewIterator(ImageView *,ImageView *,TransferImageViewMethod,
    void *),
  UpdateImageViewIterator(ImageView *,UpdateImageViewMethod,void *);

extern MagickExport PixelPacket
  *GetImageViewAuthenticPixels(const ImageView *);

extern MagickExport size_t
  GetImageViewHeight(const ImageView *),
  GetImageViewWidth(const ImageView *);

extern MagickExport ssize_t
  GetImageViewX(const ImageView *),
  GetImageViewY(const ImageView *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
