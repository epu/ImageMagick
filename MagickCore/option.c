/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   OOO   PPPP   TTTTT  IIIII   OOO   N   N                   %
%                  O   O  P   P    T      I    O   O  NN  N                   %
%                  O   O  PPPP     T      I    O   O  N N N                   %
%                  O   O  P        T      I    O   O  N  NN                   %
%                   OOO   P        T    IIIII   OOO   N   N                   %
%                                                                             %
%                                                                             %
%                         MagickCore Option Methods                           %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
%                                                                             %
%                                                                             %
%  Copyright 1999-2013 ImageMagick Studio LLC, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the License.  You may  %
%  obtain a copy of the License at                                            %
%                                                                             %
%    http://www.imagemagick.org/script/license.php                            %
%                                                                             %
%  Unless required by applicable law or agreed to in writing, software        %
%  distributed under the License is distributed on an "AS IS" BASIS,          %
%  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   %
%  See the License for the specific language governing permissions and        %
%  limitations under the License.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "MagickCore/studio.h"
#include "MagickCore/artifact.h"
#include "MagickCore/cache.h"
#include "MagickCore/color.h"
#include "MagickCore/compare.h"
#include "MagickCore/constitute.h"
#include "MagickCore/distort.h"
#include "MagickCore/draw.h"
#include "MagickCore/effect.h"
#include "MagickCore/exception.h"
#include "MagickCore/exception-private.h"
#include "MagickCore/fx.h"
#include "MagickCore/gem.h"
#include "MagickCore/geometry.h"
#include "MagickCore/image.h"
#include "MagickCore/image-private.h"
#include "MagickCore/layer.h"
#include "MagickCore/mime-private.h"
#include "MagickCore/memory_.h"
#include "MagickCore/monitor.h"
#include "MagickCore/montage.h"
#include "MagickCore/morphology.h"
#include "MagickCore/option.h"
#include "MagickCore/pixel.h"
#include "MagickCore/policy.h"
#include "MagickCore/property.h"
#include "MagickCore/quantize.h"
#include "MagickCore/quantum.h"
#include "MagickCore/resample.h"
#include "MagickCore/resource_.h"
#include "MagickCore/splay-tree.h"
#include "MagickCore/statistic.h"
#include "MagickCore/string_.h"
#include "MagickCore/token.h"
#include "MagickCore/utility.h"

/*
  ImageMagick options.
*/
static const OptionInfo
  AlignOptions[] =
  {
    { "Undefined", UndefinedAlign, UndefinedOptionFlag, MagickTrue },
    { "Center", CenterAlign, UndefinedOptionFlag, MagickFalse },
    { "End", RightAlign, UndefinedOptionFlag, MagickFalse },
    { "Left", LeftAlign, UndefinedOptionFlag, MagickFalse },
    { "Middle", CenterAlign, UndefinedOptionFlag, MagickFalse },
    { "Right", RightAlign, UndefinedOptionFlag, MagickFalse },
    { "Start", LeftAlign, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedAlign, UndefinedOptionFlag, MagickFalse }
  },
  AlphaChannelOptions[] =
  {
    { "Undefined", UndefinedAlphaChannel, UndefinedOptionFlag, MagickTrue },
    { "Activate", ActivateAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Background", BackgroundAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Copy", CopyAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Deactivate", DeactivateAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Extract", ExtractAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Off", DeactivateAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "On", ActivateAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Opaque", OpaqueAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Remove", RemoveAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Set", SetAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Shape", ShapeAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Reset", SetAlphaChannel, DeprecateOptionFlag, MagickTrue },
    { "Transparent", TransparentAlphaChannel, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedAlphaChannel, UndefinedOptionFlag, MagickFalse }
  },
  BooleanOptions[] =
  {
    { "False", MagickFalse, UndefinedOptionFlag, MagickFalse },
    { "True", MagickTrue, UndefinedOptionFlag, MagickFalse },
    { "0", MagickFalse, UndefinedOptionFlag, MagickFalse },
    { "1", MagickTrue, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, MagickFalse, UndefinedOptionFlag, MagickFalse }
  },
  CacheOptions[] =
  {
    { "Disk", DiskCache, UndefinedOptionFlag, MagickFalse },
    { "Distributed", DistributedCache, UndefinedOptionFlag, MagickFalse },
    { "Map", MapCache, UndefinedOptionFlag, MagickFalse },
    { "Memory", MemoryCache, UndefinedOptionFlag, MagickFalse },
    { "Ping", PingCache, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, MagickFalse, UndefinedOptionFlag, MagickFalse }
  },
  ChannelOptions[] =
  {
    { "Undefined", UndefinedChannel, UndefinedOptionFlag, MagickTrue },
    /* special */
    { "All", CompositeChannels, UndefinedOptionFlag, MagickFalse },
    { "Sync", SyncChannels, UndefinedOptionFlag, MagickFalse },
    { "Default", DefaultChannels, UndefinedOptionFlag, MagickFalse },
    /* individual channel */
    { "A", AlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Alpha", AlphaChannel, UndefinedOptionFlag, MagickFalse },
    { "Black", BlackChannel, UndefinedOptionFlag, MagickFalse },
    { "B", BlueChannel, UndefinedOptionFlag, MagickFalse },
    { "Blue", BlueChannel, UndefinedOptionFlag, MagickFalse },
    { "C", CyanChannel, UndefinedOptionFlag, MagickFalse },
    { "Cyan", CyanChannel, UndefinedOptionFlag, MagickFalse },
    { "Gray", GrayChannel, UndefinedOptionFlag, MagickFalse },
    { "G", GreenChannel, UndefinedOptionFlag, MagickFalse },
    { "Green", GreenChannel, UndefinedOptionFlag, MagickFalse },
    { "H", RedChannel, UndefinedOptionFlag, MagickFalse },
    { "Hue", RedChannel, UndefinedOptionFlag, MagickFalse },
    { "K", BlackChannel, UndefinedOptionFlag, MagickFalse },
    { "L", BlueChannel, UndefinedOptionFlag, MagickFalse },
    { "Lightness", BlueChannel, UndefinedOptionFlag, MagickFalse },
    { "Luminance", BlueChannel, UndefinedOptionFlag, MagickFalse },
    { "Luminosity", BlueChannel, DeprecateOptionFlag, MagickTrue },
    { "M", MagentaChannel, UndefinedOptionFlag, MagickFalse },
    { "Magenta", MagentaChannel, UndefinedOptionFlag, MagickFalse },
    { "Matte", AlphaChannel, DeprecateOptionFlag, MagickTrue },/*depreciate*/
    { "Opacity", AlphaChannel, DeprecateOptionFlag, MagickTrue },/*depreciate*/
    { "R", RedChannel, UndefinedOptionFlag, MagickFalse },
    { "Red", RedChannel, UndefinedOptionFlag, MagickFalse },
    { "S", GreenChannel, UndefinedOptionFlag, MagickFalse },
    { "Saturation", GreenChannel, UndefinedOptionFlag, MagickFalse },
    { "Y", YellowChannel, UndefinedOptionFlag, MagickFalse },
    { "Yellow", YellowChannel, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedChannel, UndefinedOptionFlag, MagickFalse }
  },
  ClassOptions[] =
  {
    { "Undefined", UndefinedClass, UndefinedOptionFlag, MagickTrue },
    { "DirectClass", DirectClass, UndefinedOptionFlag, MagickFalse },
    { "PseudoClass", PseudoClass, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedClass, UndefinedOptionFlag, MagickFalse }
  },
  ClipPathOptions[] =
  {
    { "Undefined", UndefinedPathUnits, UndefinedOptionFlag, MagickTrue },
    { "ObjectBoundingBox", ObjectBoundingBox, UndefinedOptionFlag, MagickFalse },
    { "UserSpace", UserSpace, UndefinedOptionFlag, MagickFalse },
    { "UserSpaceOnUse", UserSpaceOnUse, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPathUnits, UndefinedOptionFlag, MagickFalse }
  },
  CommandOptions[] =
  {
    /* WARNING: this must be sorted by name, then by switch character
       So that it can be referenced using a binary search for speed.
       See GetCommandOptionInfo() below for details.

       Check on sort...
           magick -list command > t1
           sort -k 1.2  t1 | diff t1 -
       Should not show any differences...
    */
    { "(", 0L, NoImageOperatorFlag, MagickTrue },
    { ")", 0L, NoImageOperatorFlag, MagickTrue },
    { "{", 0L, NoImageOperatorFlag, MagickTrue },
    { "}", 0L, NoImageOperatorFlag, MagickTrue },
    { "--", 1L, NoImageOperatorFlag, MagickTrue },
    { "+adaptive-blur", 1L, DeprecateOptionFlag, MagickTrue },
    { "-adaptive-blur", 1L, SimpleOperatorFlag, MagickFalse },
    { "+adaptive-resize", 1L, DeprecateOptionFlag, MagickTrue },
    { "-adaptive-resize", 1L, SimpleOperatorFlag, MagickFalse },
    { "+adaptive-sharpen", 1L, DeprecateOptionFlag, MagickTrue },
    { "-adaptive-sharpen", 1L, SimpleOperatorFlag, MagickFalse },
    { "+adjoin", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-adjoin", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+affine", 0L, ReplacedOptionFlag | DrawInfoOptionFlag, MagickTrue },
    { "-affine", 1L, ReplacedOptionFlag | DrawInfoOptionFlag, MagickTrue },
    { "+affinity", 0L, DeprecateOptionFlag, MagickTrue },
    { "-affinity", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "+alpha", 1L, DeprecateOptionFlag, MagickTrue },
    { "-alpha", 1L, SimpleOperatorFlag, MagickFalse },
    { "+annotate", 0L, DeprecateOptionFlag, MagickTrue },
    { "-annotate", 2L, SimpleOperatorFlag | AlwaysInterpretArgsFlag, MagickFalse },
    { "+antialias", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-antialias", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+append", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-append", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+attenuate", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-attenuate", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+authenticate", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-authenticate", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+auto-gamma", 0L, DeprecateOptionFlag, MagickTrue },
    { "-auto-gamma", 0L, SimpleOperatorFlag, MagickFalse },
    { "+auto-level", 0L, DeprecateOptionFlag, MagickTrue },
    { "-auto-level", 0L, SimpleOperatorFlag, MagickFalse },
    { "+auto-orient", 0L, DeprecateOptionFlag, MagickTrue },
    { "-auto-orient", 0L, SimpleOperatorFlag, MagickFalse },
    { "+average", 0L, DeprecateOptionFlag, MagickTrue },
    { "-average", 0L, ReplacedOptionFlag | ListOperatorFlag | FireOptionFlag, MagickTrue },
    { "+backdrop", 0L, NonMagickOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-backdrop", 1L, NonMagickOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+background", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-background", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+bench", 1L, DeprecateOptionFlag, MagickTrue },
    { "-bench", 1L, GenesisOptionFlag, MagickFalse },
    { "+bias", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-bias", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+black-point-compensation", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-black-point-compensation", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+black-threshold", 0L, DeprecateOptionFlag, MagickTrue },
    { "-black-threshold", 1L, SimpleOperatorFlag, MagickFalse },
    { "+blend", 0L, NonMagickOptionFlag, MagickFalse },
    { "-blend", 1L, NonMagickOptionFlag, MagickFalse },
    { "+blue-primary", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-blue-primary", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+blue-shift", 1L, SimpleOperatorFlag, MagickFalse },
    { "-blue-shift", 1L, SimpleOperatorFlag, MagickFalse },
    { "+blur", 0L, DeprecateOptionFlag, MagickTrue },
    { "-blur", 1L, SimpleOperatorFlag, MagickFalse },
    { "+border", 1L, DeprecateOptionFlag, MagickTrue },
    { "-border", 1L, SimpleOperatorFlag, MagickFalse },
    { "+bordercolor", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-bordercolor", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+borderwidth", 0L, NonMagickOptionFlag, MagickFalse },
    { "-borderwidth", 1L, NonMagickOptionFlag, MagickFalse },
    { "+box", 0L, ReplacedOptionFlag | ImageInfoOptionFlag | DrawInfoOptionFlag, MagickTrue },
    { "-box", 1L, ReplacedOptionFlag | ImageInfoOptionFlag | DrawInfoOptionFlag, MagickTrue },
    { "+brightness-contrast", 0L, DeprecateOptionFlag, MagickTrue },
    { "-brightness-contrast", 1L, SimpleOperatorFlag, MagickFalse },
    { "+cache", 0L, GlobalOptionFlag, MagickFalse },
    { "-cache", 1L, GlobalOptionFlag, MagickFalse },
    { "+caption", 0L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-caption", 1L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+cdl", 1L, DeprecateOptionFlag, MagickTrue },
    { "-cdl", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+channel", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-channel", 1L, ImageInfoOptionFlag, MagickFalse },
    { "-channel-fx", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+charcoal", 1L, DeprecateOptionFlag, MagickTrue },
    { "-charcoal", 1L, SimpleOperatorFlag, MagickFalse },
    { "+chop", 1L, DeprecateOptionFlag, MagickTrue },
    { "-chop", 1L, SimpleOperatorFlag, MagickFalse },
    { "+clamp", 0L, DeprecateOptionFlag, MagickTrue },
    { "-clamp", 0L, SimpleOperatorFlag, MagickFalse },
    { "+clip", 0L, SimpleOperatorFlag, MagickFalse },
    { "-clip", 0L, SimpleOperatorFlag, MagickFalse },
    { "+clip-mask", 0L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-clip-mask", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+clip-path", 1L, SimpleOperatorFlag, MagickFalse },
    { "-clip-path", 1L, SimpleOperatorFlag, MagickFalse },
    { "+clone", 0L, NoImageOperatorFlag, MagickFalse },
    { "-clone", 1L, NoImageOperatorFlag, MagickFalse },
    { "+clut", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-clut", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+coalesce", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-coalesce", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+color-matrix", 1L, DeprecateOptionFlag, MagickTrue },
    { "-color-matrix", 1L, SimpleOperatorFlag, MagickFalse },
    { "+colorize", 1L, DeprecateOptionFlag, MagickTrue },
    { "-colorize", 1L, SimpleOperatorFlag, MagickFalse },
    { "+colormap", 0L, NonMagickOptionFlag, MagickFalse },
    { "-colormap", 1L, NonMagickOptionFlag, MagickFalse },
    { "+colors", 1L, DeprecateOptionFlag, MagickTrue },
    { "-colors", 1L, SimpleOperatorFlag, MagickFalse },
    { "+colorspace", 0L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "-colorspace", 1L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "+combine", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-combine", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+comment", 0L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-comment", 1L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+compose", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-compose", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+composite", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-composite", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+compress", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-compress", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+concurrent", 0L, DeprecateOptionFlag, MagickTrue },
    { "-concurrent", 0L, GenesisOptionFlag, MagickFalse },
    { "+contrast", 0L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "-contrast", 0L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+contrast-stretch", 1L, DeprecateOptionFlag, MagickTrue },
    { "-contrast-stretch", 1L, SimpleOperatorFlag, MagickFalse },
    { "+convolve", 1L, DeprecateOptionFlag, MagickTrue },
    { "-convolve", 1L, SimpleOperatorFlag, MagickFalse },
    { "+crop", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-crop", 1L, SimpleOperatorFlag | FireOptionFlag, MagickFalse },
    { "+cycle", 1L, DeprecateOptionFlag, MagickTrue },
    { "-cycle", 1L, SimpleOperatorFlag, MagickFalse },
    { "+debug", 0L, GlobalOptionFlag | FireOptionFlag, MagickFalse },
    { "-debug", 1L, GlobalOptionFlag | FireOptionFlag, MagickFalse },
    { "+decipher", 1L, DeprecateOptionFlag, MagickTrue },
    { "-decipher", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+deconstruct", 0L, DeprecateOptionFlag, MagickTrue },
    { "-deconstruct", 0L, ReplacedOptionFlag | ListOperatorFlag | FireOptionFlag, MagickTrue },
    { "+define", 1L, ImageInfoOptionFlag | FireOptionFlag, MagickFalse },
    { "-define", 1L, ImageInfoOptionFlag | FireOptionFlag, MagickFalse },
    { "+delay", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-delay", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+delete", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-delete", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+density", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-density", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+depth", 0L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "-depth", 1L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "+descend", 0L, NonMagickOptionFlag, MagickFalse },
    { "-descend", 1L, NonMagickOptionFlag, MagickFalse },
    { "+deskew", 0L, SimpleOperatorFlag, MagickFalse },
    { "-deskew", 1L, SimpleOperatorFlag, MagickFalse },
    { "+despeckle", 0L, DeprecateOptionFlag, MagickTrue },
    { "-despeckle", 0L, SimpleOperatorFlag, MagickFalse },
    { "+direction", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-direction", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+displace", 0L, NonMagickOptionFlag, MagickFalse },
    { "-displace", 1L, NonMagickOptionFlag, MagickFalse },
    { "+display", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-display", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+dispose", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-dispose", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+dissolve", 0L, NonMagickOptionFlag, MagickFalse },
    { "-dissolve", 1L, NonMagickOptionFlag, MagickFalse },
    { "+dissimilarity-threshold", 0L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "-dissimilarity-threshold", 1L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "+distort", 2L, SimpleOperatorFlag | AlwaysInterpretArgsFlag, MagickFalse },
    { "-distort", 2L, SimpleOperatorFlag | AlwaysInterpretArgsFlag, MagickFalse },
    { "+dither", 0L, ImageInfoOptionFlag | QuantizeInfoOptionFlag, MagickFalse },
    { "-dither", 1L, ImageInfoOptionFlag | QuantizeInfoOptionFlag, MagickFalse },
    { "+draw", 0L, DeprecateOptionFlag, MagickTrue },
    { "-draw", 1L, SimpleOperatorFlag, MagickFalse },
    { "+duplicate", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-duplicate", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+duration", 1L, GenesisOptionFlag, MagickFalse },
    { "-duration", 1L, GenesisOptionFlag, MagickFalse },
    { "+edge", 1L, DeprecateOptionFlag, MagickTrue },
    { "-edge", 1L, SimpleOperatorFlag, MagickFalse },
    { "+emboss", 1L, DeprecateOptionFlag, MagickTrue },
    { "-emboss", 1L, SimpleOperatorFlag, MagickFalse },
    { "+encipher", 1L, DeprecateOptionFlag, MagickTrue },
    { "-encipher", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+encoding", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-encoding", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+endian", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-endian", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+enhance", 0L, DeprecateOptionFlag, MagickTrue },
    { "-enhance", 0L, SimpleOperatorFlag, MagickFalse },
    { "+equalize", 0L, DeprecateOptionFlag, MagickTrue },
    { "-equalize", 0L, SimpleOperatorFlag, MagickFalse },
    { "+evaluate", 2L, DeprecateOptionFlag, MagickTrue },
    { "-evaluate", 2L, SimpleOperatorFlag, MagickFalse },
    { "+evaluate-sequence", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-evaluate-sequence", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-exit", 0L, SpecialOptionFlag, MagickFalse },
    { "+extent", 1L, DeprecateOptionFlag, MagickTrue },
    { "-extent", 1L, SimpleOperatorFlag, MagickFalse },
    { "+extract", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-extract", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+family", 0L, DeprecateOptionFlag, MagickTrue },
    { "-family", 1L, DrawInfoOptionFlag, MagickFalse },
    { "+features", 0L, SimpleOperatorFlag | FireOptionFlag, MagickFalse },
    { "-features", 1L, SimpleOperatorFlag | FireOptionFlag, MagickFalse },
    { "+fft", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-fft", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+fill", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-fill", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+filter", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-filter", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+flatten", 0L, DeprecateOptionFlag, MagickTrue },
    { "-flatten", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+flip", 0L, DeprecateOptionFlag, MagickTrue },
    { "-flip", 0L, SimpleOperatorFlag, MagickFalse },
    { "+floodfill", 2L, SimpleOperatorFlag, MagickFalse },
    { "-floodfill", 2L, SimpleOperatorFlag, MagickFalse },
    { "+flop", 0L, DeprecateOptionFlag, MagickTrue },
    { "-flop", 0L, SimpleOperatorFlag, MagickFalse },
    { "+font", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-font", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+foreground", 0L, NonMagickOptionFlag, MagickFalse },
    { "-foreground", 1L, NonMagickOptionFlag, MagickFalse },
    { "+format", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-format", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+frame", 1L, DeprecateOptionFlag, MagickTrue },
    { "-frame", 1L, SimpleOperatorFlag, MagickFalse },
    { "+function", 2L, DeprecateOptionFlag, MagickTrue },
    { "-function", 2L,SimpleOperatorFlag | AlwaysInterpretArgsFlag, MagickFalse },
    { "+fuzz", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-fuzz", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+fx", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-fx", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+gamma", 1L, SimpleOperatorFlag, MagickFalse },
    { "-gamma", 1L, SimpleOperatorFlag, MagickFalse },
    { "+gaussian", 1L, DeprecateOptionFlag, MagickTrue },
    { "-gaussian", 1L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+gaussian-blur", 1L, DeprecateOptionFlag, MagickTrue },
    { "-gaussian-blur", 1L, SimpleOperatorFlag, MagickFalse },
    { "+geometry", 0L, SimpleOperatorFlag, MagickFalse },
    { "-geometry", 1L, SimpleOperatorFlag, MagickFalse },
    { "+gravity", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-gravity", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+grayscale", 1L, SimpleOperatorFlag, MagickTrue },
    { "-grayscale", 1L, SimpleOperatorFlag, MagickFalse },
    { "+green-primary", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-green-primary", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+hald-clut", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-hald-clut", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+highlight-color", 0L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "-highlight-color", 1L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "+iconGeometry", 0L, NonMagickOptionFlag, MagickFalse },
    { "-iconGeometry", 1L, NonMagickOptionFlag, MagickFalse },
    { "+iconic", 0L, NonMagickOptionFlag, MagickFalse },
    { "-iconic", 1L, NonMagickOptionFlag, MagickFalse },
    { "+identify", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-identify", 0L, SimpleOperatorFlag | FireOptionFlag, MagickFalse },
    { "+ift", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-ift", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+immutable", 0L, NonMagickOptionFlag, MagickFalse },
    { "-immutable", 0L, NonMagickOptionFlag, MagickFalse },
    { "+implode", 0L, DeprecateOptionFlag, MagickTrue },
    { "-implode", 1L, SimpleOperatorFlag, MagickFalse },
    { "+insert", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-insert", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+intensity", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-intensity", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+intent", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-intent", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+interlace", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-interlace", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+interline-spacing", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-interline-spacing", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+interpolate", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-interpolate", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+interpolative-resize", 1L, DeprecateOptionFlag, MagickTrue },
    { "-interpolative-resize", 1L, SimpleOperatorFlag, MagickFalse },
    { "+interword-spacing", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-interword-spacing", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+kerning", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-kerning", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+label", 0L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-label", 1L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+lat", 1L, DeprecateOptionFlag, MagickTrue },
    { "-lat", 1L, SimpleOperatorFlag, MagickFalse },
    { "+layers", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-layers", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+level", 1L, SimpleOperatorFlag, MagickFalse },
    { "-level", 1L, SimpleOperatorFlag, MagickFalse },
    { "+level-colors", 1L, SimpleOperatorFlag, MagickFalse },
    { "-level-colors", 1L, SimpleOperatorFlag, MagickFalse },
    { "+limit", 0L, DeprecateOptionFlag, MagickTrue },
    { "-limit", 2L, GlobalOptionFlag | FireOptionFlag, MagickFalse },
    { "+linear-stretch", 1L, DeprecateOptionFlag, MagickTrue },
    { "-linear-stretch", 1L, SimpleOperatorFlag, MagickFalse },
    { "+liquid-rescale", 1L, DeprecateOptionFlag, MagickTrue },
    { "-liquid-rescale", 1L, SimpleOperatorFlag, MagickFalse },
    { "+list", 0L, DeprecateOptionFlag, MagickTrue },
    { "-list", 1L, NoImageOperatorFlag, MagickFalse },
    { "+log", 0L, DeprecateOptionFlag, MagickFalse },
    { "-log", 1L, GlobalOptionFlag, MagickFalse },
    { "+loop", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-loop", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+lowlight-color", 0L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "-lowlight-color", 1L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "+magnify", 0L, NonMagickOptionFlag, MagickFalse },
    { "-magnify", 1L, NonMagickOptionFlag, MagickFalse },
    { "+map", 0L, ReplacedOptionFlag | ListOperatorFlag | FireOptionFlag, MagickTrue },
    { "-map", 1L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+mask", 0L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-mask", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+matte", 0L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "-matte", 0L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+mattecolor", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-mattecolor", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+maximum", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-maximum", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "+median", 1L, DeprecateOptionFlag, MagickTrue },
    { "-median", 1L, ReplacedOptionFlag | SimpleOperatorFlag | FireOptionFlag, MagickTrue },
    { "+metric", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-metric", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+minimum", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-minimum", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "+mode", 1L, NonMagickOptionFlag, MagickFalse },
    { "-mode", 1L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+modulate", 1L, DeprecateOptionFlag, MagickTrue },
    { "-modulate", 1L, SimpleOperatorFlag, MagickFalse },
    { "+monitor", 0L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "-monitor", 0L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "+monochrome", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-monochrome", 0L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "+morph", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-morph", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+morphology", 2L, DeprecateOptionFlag, MagickTrue },
    { "-morphology", 2L, SimpleOperatorFlag, MagickFalse },
    { "+mosaic", 0L, DeprecateOptionFlag, MagickTrue },
    { "-mosaic", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+motion-blur", 1L, DeprecateOptionFlag, MagickTrue },
    { "-motion-blur", 1L, SimpleOperatorFlag, MagickFalse },
    { "+name", 0L, NonMagickOptionFlag, MagickFalse },
    { "-name", 1L, NonMagickOptionFlag, MagickFalse },
    { "+negate", 0L, SimpleOperatorFlag, MagickFalse },
    { "-negate", 0L, SimpleOperatorFlag, MagickFalse },
    { "+noise", 1L, SimpleOperatorFlag, MagickFalse },
    { "-noise", 1L, ReplacedOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "-noop", 0L, NoImageOperatorFlag, MagickFalse },
    { "+normalize", 0L, DeprecateOptionFlag, MagickTrue },
    { "-normalize", 0L, SimpleOperatorFlag, MagickFalse },
    { "+opaque", 1L, SimpleOperatorFlag, MagickFalse },
    { "-opaque", 1L, SimpleOperatorFlag, MagickFalse },
    { "+ordered-dither", 0L, DeprecateOptionFlag, MagickTrue },
    { "-ordered-dither", 1L, SimpleOperatorFlag, MagickFalse },
    { "+orient", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-orient", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+page", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-page", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+paint", 0L, DeprecateOptionFlag, MagickTrue },
    { "-paint", 1L, SimpleOperatorFlag, MagickFalse },
    { "+path", 0L, NonMagickOptionFlag, MagickFalse },
    { "-path", 1L, NonMagickOptionFlag, MagickFalse },
    { "+pause", 0L, NonMagickOptionFlag, MagickFalse },
    { "-pause", 1L, NonMagickOptionFlag, MagickFalse },
    { "+ping", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-ping", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+pointsize", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-pointsize", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+polaroid", 0L, SimpleOperatorFlag, MagickFalse },
    { "-polaroid", 1L, SimpleOperatorFlag, MagickFalse },
    { "+poly", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-poly", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+posterize", 1L, DeprecateOptionFlag, MagickTrue },
    { "-posterize", 1L, SimpleOperatorFlag, MagickFalse },
    { "+precision", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-precision", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+preview", 0L, DeprecateOptionFlag, MagickTrue },
    { "-preview", 1L, GlobalOptionFlag, MagickFalse },
    { "+print", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-print", 1L, ListOperatorFlag | AlwaysInterpretArgsFlag | FireOptionFlag, MagickFalse },
    { "+process", 1L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-process", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+profile", 1L, SimpleOperatorFlag, MagickFalse },
    { "-profile", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+quality", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-quality", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+quantize", 0L, QuantizeInfoOptionFlag, MagickFalse },
    { "-quantize", 1L, QuantizeInfoOptionFlag, MagickFalse },
    { "+quiet", 0L, GlobalOptionFlag | FireOptionFlag, MagickFalse },
    { "-quiet", 0L, GlobalOptionFlag | FireOptionFlag, MagickFalse },
    { "+radial-blur", 1L, DeprecateOptionFlag, MagickTrue },
    { "-radial-blur", 1L, SimpleOperatorFlag, MagickFalse },
    { "+raise", 1L, SimpleOperatorFlag, MagickFalse },
    { "-raise", 1L, SimpleOperatorFlag, MagickFalse },
    { "+random-threshold", 1L, DeprecateOptionFlag, MagickTrue },
    { "-random-threshold", 1L, SimpleOperatorFlag, MagickFalse },
    { "-read", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+recolor", 1L, DeprecateOptionFlag, MagickTrue },
    { "-recolor", 1L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+red-primary", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-red-primary", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+regard-warnings", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-regard-warnings", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+region", 0L, NoImageOperatorFlag, MagickFalse },
    { "-region", 1L, NoImageOperatorFlag, MagickFalse },
    { "+remap", 0L, ListOperatorFlag | NeverInterpretArgsFlag | FireOptionFlag, MagickFalse },
    { "-remap", 1L, SimpleOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+remote", 0L, NonMagickOptionFlag, MagickFalse },
    { "-remote", 1L, NonMagickOptionFlag, MagickFalse },
    { "+render", 0L, DrawInfoOptionFlag, MagickFalse },
    { "-render", 0L, DrawInfoOptionFlag, MagickFalse },
    { "+repage", 0L, SimpleOperatorFlag, MagickFalse },
    { "-repage", 1L, SimpleOperatorFlag, MagickFalse },
    { "+resample", 1L, DeprecateOptionFlag, MagickTrue },
    { "-resample", 1L, SimpleOperatorFlag, MagickFalse },
    { "+resize", 1L, DeprecateOptionFlag, MagickTrue },
    { "-resize", 1L, SimpleOperatorFlag, MagickFalse },
    { "+respect-parenthesis", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-respect-parenthesis", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+reverse", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-reverse", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+roll", 1L, DeprecateOptionFlag, MagickTrue },
    { "-roll", 1L, SimpleOperatorFlag, MagickFalse },
    { "+rotate", 1L, DeprecateOptionFlag, MagickTrue },
    { "-rotate", 1L, SimpleOperatorFlag, MagickFalse },
    { "+sample", 1L, DeprecateOptionFlag, MagickTrue },
    { "-sample", 1L, SimpleOperatorFlag, MagickFalse },
    { "+sampling-factor", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-sampling-factor", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+sans", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue },
    { "-sans", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue },
    { "+sans0", 0L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue }, /* equivelent to 'noop' */
    { "-sans0", 0L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue },
    { "+sans1", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue },
    { "-sans1", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue }, /* equivelent to 'sans' */
    { "+sans2", 2L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue },
    { "-sans2", 2L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickTrue },
    { "+scale", 1L, DeprecateOptionFlag, MagickTrue },
    { "-scale", 1L, SimpleOperatorFlag, MagickFalse },
    { "+scene", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-scene", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+scenes", 0L, NonMagickOptionFlag, MagickFalse },
    { "-scenes", 1L, NonMagickOptionFlag, MagickFalse },
    { "+screen", 0L, NonMagickOptionFlag, MagickFalse },
    { "-screen", 1L, NonMagickOptionFlag, MagickFalse },
    { "-script", 1L, SpecialOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+seed", 0L, GlobalOptionFlag, MagickFalse },
    { "-seed", 1L, GlobalOptionFlag, MagickFalse },
    { "+segment", 1L, DeprecateOptionFlag, MagickTrue },
    { "-segment", 1L, SimpleOperatorFlag, MagickFalse },
    { "+selective-blur", 1L, DeprecateOptionFlag, MagickTrue },
    { "-selective-blur", 1L, SimpleOperatorFlag, MagickFalse },
    { "+separate", 0L, DeprecateOptionFlag | FireOptionFlag, MagickTrue },
    { "-separate", 0L, SimpleOperatorFlag | FireOptionFlag, MagickFalse },
    { "+sepia-tone", 1L, DeprecateOptionFlag, MagickTrue },
    { "-sepia-tone", 1L, SimpleOperatorFlag, MagickFalse },
    { "+set", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-set", 2L, NoImageOperatorFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+shade", 0L, DeprecateOptionFlag, MagickTrue },
    { "-shade", 1L, SimpleOperatorFlag, MagickFalse },
    { "+shadow", 1L, DeprecateOptionFlag, MagickTrue },
    { "-shadow", 1L, SimpleOperatorFlag, MagickFalse },
    { "+shared-memory", 0L, NonMagickOptionFlag, MagickFalse },
    { "-shared-memory", 1L, NonMagickOptionFlag, MagickFalse },
    { "+sharpen", 1L, DeprecateOptionFlag, MagickTrue },
    { "-sharpen", 1L, SimpleOperatorFlag, MagickFalse },
    { "+shave", 1L, DeprecateOptionFlag, MagickTrue },
    { "-shave", 1L, SimpleOperatorFlag, MagickFalse },
    { "+shear", 1L, DeprecateOptionFlag, MagickTrue },
    { "-shear", 1L, SimpleOperatorFlag, MagickFalse },
    { "+sigmoidal-contrast", 1L, SimpleOperatorFlag, MagickFalse },
    { "-sigmoidal-contrast", 1L, SimpleOperatorFlag, MagickFalse },
    { "+silent", 0L, NonMagickOptionFlag, MagickFalse },
    { "-silent", 1L, NonMagickOptionFlag, MagickFalse },
    { "+similarity-threshold", 0L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "-similarity-threshold", 1L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "+size", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-size", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+sketch", 1L, DeprecateOptionFlag, MagickTrue },
    { "-sketch", 1L, SimpleOperatorFlag, MagickFalse },
    { "+smush", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-smush", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+snaps", 0L, NonMagickOptionFlag, MagickFalse },
    { "-snaps", 1L, NonMagickOptionFlag, MagickFalse },
    { "+solarize", 1L, DeprecateOptionFlag, MagickTrue },
    { "-solarize", 1L, SimpleOperatorFlag, MagickFalse },
    { "+sparse-color", 1L, DeprecateOptionFlag, MagickTrue },
    { "-sparse-color", 2L, SimpleOperatorFlag | AlwaysInterpretArgsFlag, MagickFalse },
    { "+splice", 1L, DeprecateOptionFlag, MagickTrue },
    { "-splice", 1L, SimpleOperatorFlag, MagickFalse },
    { "+spread", 1L, DeprecateOptionFlag, MagickTrue },
    { "-spread", 1L, SimpleOperatorFlag, MagickFalse },
    { "+statistic", 2L, DeprecateOptionFlag, MagickTrue },
    { "-statistic", 2L, SimpleOperatorFlag, MagickFalse },
    { "+stegano", 0L, NonMagickOptionFlag, MagickFalse },
    { "-stegano", 1L, NonMagickOptionFlag, MagickFalse },
    { "+stereo", 0L, DeprecateOptionFlag, MagickTrue },
    { "-stereo", 1L, NonMagickOptionFlag, MagickFalse },
    { "+stretch", 1L, DeprecateOptionFlag, MagickTrue },
    { "-stretch", 1L, SimpleOperatorFlag, MagickFalse },
    { "+strip", 0L, DeprecateOptionFlag, MagickTrue },
    { "-strip", 0L, SimpleOperatorFlag, MagickFalse },
    { "+stroke", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-stroke", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+strokewidth", 1L, ImageInfoOptionFlag, MagickFalse },
    { "-strokewidth", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+style", 0L, DrawInfoOptionFlag, MagickFalse },
    { "-style", 1L, DrawInfoOptionFlag, MagickFalse },
    { "+subimage-search", 0L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "-subimage-search", 0L, NonMagickOptionFlag | ImageInfoOptionFlag, MagickFalse },
    { "-subimage", 0L, ListOperatorFlag, MagickFalse },
    { "+swap", 0L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "-swap", 1L, ListOperatorFlag | FireOptionFlag, MagickFalse },
    { "+swirl", 1L, DeprecateOptionFlag, MagickTrue },
    { "-swirl", 1L, SimpleOperatorFlag, MagickFalse },
    { "+synchronize", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-synchronize", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+taint", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-taint", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+text-font", 0L, NonMagickOptionFlag, MagickFalse },
    { "-text-font", 1L, NonMagickOptionFlag, MagickFalse },
    { "+texture", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-texture", 1L, ImageInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+threshold", 0L, SimpleOperatorFlag, MagickFalse },
    { "-threshold", 1L, SimpleOperatorFlag, MagickFalse },
    { "+thumbnail", 1L, DeprecateOptionFlag, MagickTrue },
    { "-thumbnail", 1L, SimpleOperatorFlag, MagickFalse },
    { "+tile", 0L, DrawInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "-tile", 1L, DrawInfoOptionFlag | NeverInterpretArgsFlag, MagickFalse },
    { "+tile-offset", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-tile-offset", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+tint", 1L, SimpleOperatorFlag, MagickFalse },
    { "-tint", 1L, SimpleOperatorFlag, MagickFalse },
    { "+title", 0L, NonMagickOptionFlag, MagickFalse },
    { "-title", 1L, NonMagickOptionFlag, MagickFalse },
    { "+transform", 0L, DeprecateOptionFlag, MagickTrue },
    { "-transform", 0L, ReplacedOptionFlag | SimpleOperatorFlag, MagickTrue },
    { "+transparent", 1L, SimpleOperatorFlag, MagickFalse },
    { "-transparent", 1L, SimpleOperatorFlag, MagickFalse },
    { "+transparent-color", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-transparent-color", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+transpose", 0L, DeprecateOptionFlag, MagickTrue },
    { "-transpose", 0L, SimpleOperatorFlag, MagickFalse },
    { "+transverse", 0L, DeprecateOptionFlag, MagickTrue },
    { "-transverse", 0L, SimpleOperatorFlag, MagickFalse },
    { "+treedepth", 1L, DeprecateOptionFlag, MagickTrue },
    { "-treedepth", 1L, QuantizeInfoOptionFlag, MagickFalse },
    { "+trim", 0L, DeprecateOptionFlag, MagickTrue },
    { "-trim", 0L, SimpleOperatorFlag, MagickFalse },
    { "+type", 0L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "-type", 1L, ImageInfoOptionFlag | SimpleOperatorFlag, MagickFalse },
    { "+undercolor", 0L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "-undercolor", 1L, ImageInfoOptionFlag | DrawInfoOptionFlag, MagickFalse },
    { "+unique", 0L, SimpleOperatorFlag, MagickFalse },
    { "-unique", 0L, SimpleOperatorFlag, MagickFalse },
    { "+unique-colors", 0L, DeprecateOptionFlag, MagickTrue },
    { "-unique-colors", 0L, SimpleOperatorFlag, MagickFalse },
    { "+units", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-units", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+unsharp", 1L, DeprecateOptionFlag, MagickTrue },
    { "-unsharp", 1L, SimpleOperatorFlag, MagickFalse },
    { "+update", 0L, NonMagickOptionFlag, MagickFalse },
    { "-update", 1L, NonMagickOptionFlag, MagickFalse },
    { "+use-pixmap", 0L, NonMagickOptionFlag, MagickFalse },
    { "-use-pixmap", 1L, NonMagickOptionFlag, MagickFalse },
    { "+verbose", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-verbose", 0L, ImageInfoOptionFlag, MagickFalse },
    { "+version", 0L, DeprecateOptionFlag, MagickTrue },
    { "-version", 0L, NoImageOperatorFlag, MagickFalse },
    { "+view", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-view", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+vignette", 1L, DeprecateOptionFlag, MagickTrue },
    { "-vignette", 1L, SimpleOperatorFlag, MagickFalse },
    { "+virtual-pixel", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-virtual-pixel", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+visual", 0L, NonMagickOptionFlag, MagickFalse },
    { "-visual", 1L, NonMagickOptionFlag, MagickFalse },
    { "+watermark", 0L, NonMagickOptionFlag, MagickFalse },
    { "-watermark", 1L, NonMagickOptionFlag, MagickFalse },
    { "+wave", 1L, DeprecateOptionFlag, MagickTrue },
    { "-wave", 1L, SimpleOperatorFlag, MagickFalse },
    { "+weight", 1L, DeprecateOptionFlag, MagickTrue },
    { "-weight", 1L, DrawInfoOptionFlag, MagickFalse },
    { "+white-point", 0L, ImageInfoOptionFlag, MagickFalse },
    { "-white-point", 1L, ImageInfoOptionFlag, MagickFalse },
    { "+white-threshold", 1L, DeprecateOptionFlag, MagickTrue },
    { "-white-threshold", 1L, SimpleOperatorFlag, MagickFalse },
    { "+window", 0L, NonMagickOptionFlag, MagickFalse },
    { "-window", 1L, NonMagickOptionFlag, MagickFalse },
    { "+window-group", 0L, NonMagickOptionFlag, MagickFalse },
    { "-window-group", 1L, NonMagickOptionFlag, MagickFalse },
    { "+write", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag | FireOptionFlag, MagickFalse },
    { "-write", 1L, NoImageOperatorFlag | NeverInterpretArgsFlag | FireOptionFlag, MagickFalse },
    { (char *) NULL, 0L, UndefinedOptionFlag, MagickFalse }
  },
  ComposeOptions[] =
  {
    { "Undefined", UndefinedCompositeOp, UndefinedOptionFlag, MagickTrue },
    { "Atop", AtopCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Blend", BlendCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Blur", BlurCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Bumpmap", BumpmapCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "ChangeMask", ChangeMaskCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Clear", ClearCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "ColorBurn", ColorBurnCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "ColorDodge", ColorDodgeCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Colorize", ColorizeCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyAlpha", CopyAlphaCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyBlack", CopyBlackCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyBlue", CopyBlueCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyCyan", CopyCyanCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyGreen", CopyGreenCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Copy", CopyCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyMagenta", CopyMagentaCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyRed", CopyRedCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "CopyYellow", CopyYellowCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Darken", DarkenCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DarkenIntensity", DarkenIntensityCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DivideDst", DivideDstCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DivideSrc", DivideSrcCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Dst", DstCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Difference", DifferenceCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Displace", DisplaceCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Dissolve", DissolveCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Distort", DistortCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DstAtop", DstAtopCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DstIn", DstInCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DstOut", DstOutCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "DstOver", DstOverCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Exclusion", ExclusionCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "HardLight", HardLightCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Hue", HueCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "In", InCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Intensity", IntensityCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Lighten", LightenCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "LightenIntensity", LightenIntensityCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "LinearBurn", LinearBurnCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "LinearDodge", LinearDodgeCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "LinearLight", LinearLightCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Luminize", LuminizeCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Mathematics", MathematicsCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "MinusDst", MinusDstCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "MinusSrc", MinusSrcCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Modulate", ModulateCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "ModulusAdd", ModulusAddCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "ModulusSubtract", ModulusSubtractCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Multiply", MultiplyCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "None", NoCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Out", OutCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Overlay", OverlayCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Over", OverCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "PegtopLight", PegtopLightCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "PinLight", PinLightCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Plus", PlusCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Replace", ReplaceCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Saturate", SaturateCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Screen", ScreenCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "SoftLight", SoftLightCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Src", SrcCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "SrcAtop", SrcAtopCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "SrcIn", SrcInCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "SrcOut", SrcOutCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "SrcOver", SrcOverCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "VividLight", VividLightCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Xor", XorCompositeOp, UndefinedOptionFlag, MagickFalse },
    { "Divide", DivideDstCompositeOp, DeprecateOptionFlag, MagickTrue },
    { "Minus", MinusDstCompositeOp, DeprecateOptionFlag, MagickTrue },
    { "Threshold", ThresholdCompositeOp, DeprecateOptionFlag, MagickTrue },
    { "CopyOpacity", CopyAlphaCompositeOp, UndefinedOptionFlag, MagickTrue },
    { (char *) NULL, UndefinedCompositeOp, UndefinedOptionFlag, MagickFalse }
  },
  CompressOptions[] =
  {
    { "Undefined", UndefinedCompression, UndefinedOptionFlag, MagickTrue },
    { "B44", B44Compression, UndefinedOptionFlag, MagickFalse },
    { "B44A", B44ACompression, UndefinedOptionFlag, MagickFalse },
    { "BZip", BZipCompression, UndefinedOptionFlag, MagickFalse },
    { "DXT1", DXT1Compression, UndefinedOptionFlag, MagickFalse },
    { "DXT3", DXT3Compression, UndefinedOptionFlag, MagickFalse },
    { "DXT5", DXT5Compression, UndefinedOptionFlag, MagickFalse },
    { "Fax", FaxCompression, UndefinedOptionFlag, MagickFalse },
    { "Group4", Group4Compression, UndefinedOptionFlag, MagickFalse },
    { "JBIG1", JBIG1Compression, UndefinedOptionFlag, MagickFalse },
    { "JBIG2", JBIG2Compression, UndefinedOptionFlag, MagickFalse },
    { "JPEG", JPEGCompression, UndefinedOptionFlag, MagickFalse },
    { "JPEG2000", JPEG2000Compression, UndefinedOptionFlag, MagickFalse },
    { "Lossless", LosslessJPEGCompression, UndefinedOptionFlag, MagickFalse },
    { "LosslessJPEG", LosslessJPEGCompression, UndefinedOptionFlag, MagickFalse },
    { "LZMA", LZMACompression, UndefinedOptionFlag, MagickFalse },
    { "LZW", LZWCompression, UndefinedOptionFlag, MagickFalse },
    { "None", NoCompression, UndefinedOptionFlag, MagickFalse },
    { "Piz", PizCompression, UndefinedOptionFlag, MagickFalse },
    { "Pxr24", Pxr24Compression, UndefinedOptionFlag, MagickFalse },
    { "RLE", RLECompression, UndefinedOptionFlag, MagickFalse },
    { "Zip", ZipCompression, UndefinedOptionFlag, MagickFalse },
    { "RunlengthEncoded", RLECompression, UndefinedOptionFlag, MagickFalse },
    { "ZipS", ZipSCompression, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedCompression, UndefinedOptionFlag, MagickFalse }
  },
  ColorspaceOptions[] =
  {
    { "Undefined", UndefinedColorspace, UndefinedOptionFlag, MagickTrue },
    { "CIELab", LabColorspace, UndefinedOptionFlag, MagickFalse },
    { "CMY", CMYColorspace, UndefinedOptionFlag, MagickFalse },
    { "CMYK", CMYKColorspace, UndefinedOptionFlag, MagickFalse },
    { "Gray", GRAYColorspace, UndefinedOptionFlag, MagickFalse },
    { "HCL", HCLColorspace, UndefinedOptionFlag, MagickFalse },
    { "HSB", HSBColorspace, UndefinedOptionFlag, MagickFalse },
    { "HSL", HSLColorspace, UndefinedOptionFlag, MagickFalse },
    { "HWB", HWBColorspace, UndefinedOptionFlag, MagickFalse },
    { "Lab", LabColorspace, UndefinedOptionFlag, MagickFalse },
    { "LCH", LCHColorspace, UndefinedOptionFlag, MagickFalse },
    { "LCHab", LCHabColorspace, UndefinedOptionFlag, MagickFalse },
    { "LCHuv", LCHuvColorspace, UndefinedOptionFlag, MagickFalse },
    { "LMS", LMSColorspace, UndefinedOptionFlag, MagickFalse },
    { "Log", LogColorspace, UndefinedOptionFlag, MagickFalse },
    { "Luv", LuvColorspace, UndefinedOptionFlag, MagickFalse },
    { "OHTA", OHTAColorspace, UndefinedOptionFlag, MagickFalse },
    { "Rec601YCbCr", Rec601YCbCrColorspace, UndefinedOptionFlag, MagickFalse },
    { "Rec709YCbCr", Rec709YCbCrColorspace, UndefinedOptionFlag, MagickFalse },
    { "RGB", RGBColorspace, UndefinedOptionFlag, MagickFalse },
    { "sRGB", sRGBColorspace, UndefinedOptionFlag, MagickFalse },
    { "Transparent", TransparentColorspace, UndefinedOptionFlag, MagickFalse },
    { "XYZ", XYZColorspace, UndefinedOptionFlag, MagickFalse },
    { "YCbCr", YCbCrColorspace, UndefinedOptionFlag, MagickFalse },
    { "YCC", YCCColorspace, UndefinedOptionFlag, MagickFalse },
    { "YIQ", YIQColorspace, UndefinedOptionFlag, MagickFalse },
    { "YPbPr", YPbPrColorspace, UndefinedOptionFlag, MagickFalse },
    { "YUV", YUVColorspace, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedColorspace, UndefinedOptionFlag, MagickFalse }
  },
  DataTypeOptions[] =
  {
    { "Undefined", UndefinedData, UndefinedOptionFlag, MagickTrue },
    { "Byte", ByteData, UndefinedOptionFlag, MagickFalse },
    { "Long", LongData, UndefinedOptionFlag, MagickFalse },
    { "Short", ShortData, UndefinedOptionFlag, MagickFalse },
    { "String", StringData, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedData, UndefinedOptionFlag, MagickFalse }
  },
  DecorateOptions[] =
  {
    { "Undefined", UndefinedDecoration, UndefinedOptionFlag, MagickTrue },
    { "LineThrough", LineThroughDecoration, UndefinedOptionFlag, MagickFalse },
    { "None", NoDecoration, UndefinedOptionFlag, MagickFalse },
    { "Overline", OverlineDecoration, UndefinedOptionFlag, MagickFalse },
    { "Underline", UnderlineDecoration, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedDecoration, UndefinedOptionFlag, MagickFalse }
  },
  DirectionOptions[] =
  {
    { "Undefined", UndefinedDirection, UndefinedOptionFlag, MagickTrue },
    { "right-to-left", RightToLeftDirection, UndefinedOptionFlag, MagickFalse },
    { "left-to-right", LeftToRightDirection, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedDirection, UndefinedOptionFlag, MagickFalse }
  },
  DisposeOptions[] =
  {
    { "Undefined", UndefinedDispose, UndefinedOptionFlag, MagickTrue },
    { "Background", BackgroundDispose, UndefinedOptionFlag, MagickFalse },
    { "None", NoneDispose, UndefinedOptionFlag, MagickFalse },
    { "Previous", PreviousDispose, UndefinedOptionFlag, MagickFalse },
    { "Undefined", UndefinedDispose, UndefinedOptionFlag, MagickFalse },
    { "0", UndefinedDispose, UndefinedOptionFlag, MagickFalse },
    { "1", NoneDispose, UndefinedOptionFlag, MagickFalse },
    { "2", BackgroundDispose, UndefinedOptionFlag, MagickFalse },
    { "3", PreviousDispose, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedDispose, UndefinedOptionFlag, MagickFalse }
  },
  DistortOptions[] =
  {
    { "Affine", AffineDistortion, UndefinedOptionFlag, MagickFalse },
    { "AffineProjection", AffineProjectionDistortion, UndefinedOptionFlag, MagickFalse },
    { "ScaleRotateTranslate", ScaleRotateTranslateDistortion, UndefinedOptionFlag, MagickFalse },
    { "SRT", ScaleRotateTranslateDistortion, UndefinedOptionFlag, MagickFalse },
    { "Perspective", PerspectiveDistortion, UndefinedOptionFlag, MagickFalse },
    { "PerspectiveProjection", PerspectiveProjectionDistortion, UndefinedOptionFlag, MagickFalse },
    { "Bilinear", BilinearForwardDistortion, UndefinedOptionFlag, MagickTrue },
    { "BilinearForward", BilinearForwardDistortion, UndefinedOptionFlag, MagickFalse },
    { "BilinearReverse", BilinearReverseDistortion, UndefinedOptionFlag, MagickFalse },
    { "Polynomial", PolynomialDistortion, UndefinedOptionFlag, MagickFalse },
    { "Arc", ArcDistortion, UndefinedOptionFlag, MagickFalse },
    { "Polar", PolarDistortion, UndefinedOptionFlag, MagickFalse },
    { "DePolar", DePolarDistortion, UndefinedOptionFlag, MagickFalse },
    { "Barrel", BarrelDistortion, UndefinedOptionFlag, MagickFalse },
    { "Cylinder2Plane", Cylinder2PlaneDistortion, UndefinedOptionFlag, MagickTrue },
    { "Plane2Cylinder", Plane2CylinderDistortion, UndefinedOptionFlag, MagickTrue },
    { "BarrelInverse", BarrelInverseDistortion, UndefinedOptionFlag, MagickFalse },
    { "Shepards", ShepardsDistortion, UndefinedOptionFlag, MagickFalse },
    { "Resize", ResizeDistortion, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedDistortion, UndefinedOptionFlag, MagickFalse }
  },
  DitherOptions[] =
  {
    { "Undefined", UndefinedDitherMethod, UndefinedOptionFlag, MagickTrue },
    { "None", NoDitherMethod, UndefinedOptionFlag, MagickFalse },
    { "FloydSteinberg", FloydSteinbergDitherMethod, UndefinedOptionFlag, MagickFalse },
    { "Riemersma", RiemersmaDitherMethod, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedEndian, UndefinedOptionFlag, MagickFalse }
  },
  EndianOptions[] =
  {
    { "Undefined", UndefinedEndian, UndefinedOptionFlag, MagickTrue },
    { "LSB", LSBEndian, UndefinedOptionFlag, MagickFalse },
    { "MSB", MSBEndian, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedEndian, UndefinedOptionFlag, MagickFalse }
  },
  EvaluateOptions[] =
  {
    { "Undefined", UndefinedEvaluateOperator, UndefinedOptionFlag, MagickTrue },
    { "Abs", AbsEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Add", AddEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "AddModulus", AddModulusEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "And", AndEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Cos", CosineEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Cosine", CosineEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Divide", DivideEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Exp", ExponentialEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Exponential", ExponentialEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "GaussianNoise", GaussianNoiseEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "ImpulseNoise", ImpulseNoiseEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "LaplacianNoise", LaplacianNoiseEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "LeftShift", LeftShiftEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Log", LogEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Max", MaxEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Mean", MeanEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Median", MedianEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Min", MinEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "MultiplicativeNoise", MultiplicativeNoiseEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Multiply", MultiplyEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Or", OrEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "PoissonNoise", PoissonNoiseEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Pow", PowEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "RightShift", RightShiftEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Set", SetEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Sin", SineEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Sine", SineEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Subtract", SubtractEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Sum", SumEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Threshold", ThresholdEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "ThresholdBlack", ThresholdBlackEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "ThresholdWhite", ThresholdWhiteEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "UniformNoise", UniformNoiseEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { "Xor", XorEvaluateOperator, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedEvaluateOperator, UndefinedOptionFlag, MagickFalse }
  },
  FillRuleOptions[] =
  {
    { "Undefined", UndefinedRule, UndefinedOptionFlag, MagickTrue },
    { "Evenodd", EvenOddRule, UndefinedOptionFlag, MagickFalse },
    { "NonZero", NonZeroRule, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedRule, UndefinedOptionFlag, MagickFalse }
  },
  FilterOptions[] =
  {
    { "Undefined", UndefinedFilter, UndefinedOptionFlag, MagickTrue },
    { "Bartlett", BartlettFilter, UndefinedOptionFlag, MagickFalse },
    { "Blackman", BlackmanFilter, UndefinedOptionFlag, MagickFalse },
    { "Bohman", BohmanFilter, UndefinedOptionFlag, MagickFalse },
    { "Box", BoxFilter, UndefinedOptionFlag, MagickFalse },
    { "Catrom", CatromFilter, UndefinedOptionFlag, MagickFalse },
    { "Cosine", CosineFilter, UndefinedOptionFlag, MagickFalse },
    { "Cubic", CubicFilter, UndefinedOptionFlag, MagickFalse },
    { "Gaussian", GaussianFilter, UndefinedOptionFlag, MagickFalse },
    { "Hamming", HammingFilter, UndefinedOptionFlag, MagickFalse },
    { "Hann", HannFilter, UndefinedOptionFlag, MagickFalse },
    { "Hanning", HannFilter, UndefinedOptionFlag, MagickTrue }, /*misspell*/
    { "Hermite", HermiteFilter, UndefinedOptionFlag, MagickFalse },
    { "Jinc", JincFilter, UndefinedOptionFlag, MagickFalse },
    { "Kaiser", KaiserFilter, UndefinedOptionFlag, MagickFalse },
    { "Lagrange", LagrangeFilter, UndefinedOptionFlag, MagickFalse },
    { "Lanczos", LanczosFilter, UndefinedOptionFlag, MagickFalse },
    { "Lanczos2", Lanczos2Filter, UndefinedOptionFlag, MagickFalse },
    { "Lanczos2Sharp", Lanczos2SharpFilter, UndefinedOptionFlag, MagickFalse },
    { "LanczosRadius", LanczosRadiusFilter, UndefinedOptionFlag, MagickFalse },
    { "LanczosSharp", LanczosSharpFilter, UndefinedOptionFlag, MagickFalse },
    { "Mitchell", MitchellFilter, UndefinedOptionFlag, MagickFalse },
    { "Parzen", ParzenFilter, UndefinedOptionFlag, MagickFalse },
    { "Point", PointFilter, UndefinedOptionFlag, MagickFalse },
    { "Quadratic", QuadraticFilter, UndefinedOptionFlag, MagickFalse },
    { "Robidoux", RobidouxFilter, UndefinedOptionFlag, MagickFalse },
    { "RobidouxSharp", RobidouxSharpFilter, UndefinedOptionFlag, MagickFalse },
    { "Sinc", SincFilter, UndefinedOptionFlag, MagickFalse },
    { "SincFast", SincFastFilter, UndefinedOptionFlag, MagickFalse },
    { "Spline", SplineFilter, UndefinedOptionFlag, MagickFalse },
    { "Triangle", TriangleFilter, UndefinedOptionFlag, MagickFalse },
    { "Welch", WelchFilter, UndefinedOptionFlag, MagickFalse },
    { "Welsh", WelchFilter, UndefinedOptionFlag, MagickTrue }, /*misspell*/
    { (char *) NULL, UndefinedFilter, UndefinedOptionFlag, MagickFalse }
  },
  FunctionOptions[] =
  {
    { "Undefined", UndefinedFunction, UndefinedOptionFlag, MagickTrue },
    { "Polynomial", PolynomialFunction, UndefinedOptionFlag, MagickFalse },
    { "Sinusoid", SinusoidFunction, UndefinedOptionFlag, MagickFalse },
    { "ArcSin", ArcsinFunction, UndefinedOptionFlag, MagickFalse },
    { "ArcTan", ArctanFunction, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedFunction, UndefinedOptionFlag, MagickFalse }
  },
  GravityOptions[] =
  {
    { "Undefined", UndefinedGravity, UndefinedOptionFlag, MagickTrue },
    { "None", UndefinedGravity, UndefinedOptionFlag, MagickFalse },
    { "Center", CenterGravity, UndefinedOptionFlag, MagickFalse },
    { "East", EastGravity, UndefinedOptionFlag, MagickFalse },
    { "Forget", ForgetGravity, UndefinedOptionFlag, MagickFalse },
    { "NorthEast", NorthEastGravity, UndefinedOptionFlag, MagickFalse },
    { "North", NorthGravity, UndefinedOptionFlag, MagickFalse },
    { "NorthWest", NorthWestGravity, UndefinedOptionFlag, MagickFalse },
    { "SouthEast", SouthEastGravity, UndefinedOptionFlag, MagickFalse },
    { "South", SouthGravity, UndefinedOptionFlag, MagickFalse },
    { "SouthWest", SouthWestGravity, UndefinedOptionFlag, MagickFalse },
    { "West", WestGravity, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedGravity, UndefinedOptionFlag, MagickFalse }
  },
  IntentOptions[] =
  {
    { "Undefined", UndefinedIntent, UndefinedOptionFlag, MagickTrue },
    { "Absolute", AbsoluteIntent, UndefinedOptionFlag, MagickFalse },
    { "Perceptual", PerceptualIntent, UndefinedOptionFlag, MagickFalse },
    { "Relative", RelativeIntent, UndefinedOptionFlag, MagickFalse },
    { "Saturation", SaturationIntent, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedIntent, UndefinedOptionFlag, MagickFalse }
  },
  InterlaceOptions[] =
  {
    { "Undefined", UndefinedInterlace, UndefinedOptionFlag, MagickTrue },
    { "Line", LineInterlace, UndefinedOptionFlag, MagickFalse },
    { "None", NoInterlace, UndefinedOptionFlag, MagickFalse },
    { "Plane", PlaneInterlace, UndefinedOptionFlag, MagickFalse },
    { "Partition", PartitionInterlace, UndefinedOptionFlag, MagickFalse },
    { "GIF", GIFInterlace, UndefinedOptionFlag, MagickFalse },
    { "JPEG", JPEGInterlace, UndefinedOptionFlag, MagickFalse },
    { "PNG", PNGInterlace, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedInterlace, UndefinedOptionFlag, MagickFalse }
  },
  InterpolateOptions[] =
  {
    { "Undefined", UndefinedInterpolatePixel, UndefinedOptionFlag, MagickTrue },
    { "Average", AverageInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Average4", AverageInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Average9", Average9InterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Average16", Average16InterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Background", BackgroundInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Bilinear", BilinearInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Blend", BlendInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Catrom", CatromInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Integer", IntegerInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Mesh", MeshInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "Nearest", NearestInterpolatePixel, UndefinedOptionFlag, MagickFalse },
    { "NearestNeighbor", NearestInterpolatePixel, UndefinedOptionFlag, MagickTrue },
    { "Spline", SplineInterpolatePixel, UndefinedOptionFlag, MagickFalse },
/*  { "Filter", FilterInterpolatePixel, UndefinedOptionFlag, MagickFalse }, */
    { (char *) NULL, UndefinedInterpolatePixel, UndefinedOptionFlag, MagickFalse }
  },
  KernelOptions[] =
  {
    { "Undefined", UndefinedKernel, UndefinedOptionFlag, MagickTrue },
    { "Unity", UnityKernel, UndefinedOptionFlag, MagickFalse },
    { "Gaussian", GaussianKernel, UndefinedOptionFlag, MagickFalse },
    { "DoG", DoGKernel, UndefinedOptionFlag, MagickFalse },
    { "LoG", LoGKernel, UndefinedOptionFlag, MagickFalse },
    { "Blur", BlurKernel, UndefinedOptionFlag, MagickFalse },
    { "Comet", CometKernel, UndefinedOptionFlag, MagickFalse },
    { "Binomial", BinomialKernel, UndefinedOptionFlag, MagickFalse },
    { "Laplacian", LaplacianKernel, UndefinedOptionFlag, MagickFalse },
    { "Sobel", SobelKernel, UndefinedOptionFlag, MagickFalse },
    { "FreiChen", FreiChenKernel, UndefinedOptionFlag, MagickFalse },
    { "Roberts", RobertsKernel, UndefinedOptionFlag, MagickFalse },
    { "Prewitt", PrewittKernel, UndefinedOptionFlag, MagickFalse },
    { "Compass", CompassKernel, UndefinedOptionFlag, MagickFalse },
    { "Kirsch", KirschKernel, UndefinedOptionFlag, MagickFalse },
    { "Diamond", DiamondKernel, UndefinedOptionFlag, MagickFalse },
    { "Square", SquareKernel, UndefinedOptionFlag, MagickFalse },
    { "Rectangle", RectangleKernel, UndefinedOptionFlag, MagickFalse },
    { "Disk", DiskKernel, UndefinedOptionFlag, MagickFalse },
    { "Octagon", OctagonKernel, UndefinedOptionFlag, MagickFalse },
    { "Plus", PlusKernel, UndefinedOptionFlag, MagickFalse },
    { "Cross", CrossKernel, UndefinedOptionFlag, MagickFalse },
    { "Ring", RingKernel, UndefinedOptionFlag, MagickFalse },
    { "Peaks", PeaksKernel, UndefinedOptionFlag, MagickFalse },
    { "Edges", EdgesKernel, UndefinedOptionFlag, MagickFalse },
    { "Corners", CornersKernel, UndefinedOptionFlag, MagickFalse },
    { "Diagonals", DiagonalsKernel, UndefinedOptionFlag, MagickFalse },
    { "LineEnds", LineEndsKernel, UndefinedOptionFlag, MagickFalse },
    { "LineJunctions", LineJunctionsKernel, UndefinedOptionFlag, MagickFalse },
    { "Ridges", RidgesKernel, UndefinedOptionFlag, MagickFalse },
    { "ConvexHull", ConvexHullKernel, UndefinedOptionFlag, MagickFalse },
    { "ThinSe", ThinSEKernel, UndefinedOptionFlag, MagickFalse },
    { "Skeleton", SkeletonKernel, UndefinedOptionFlag, MagickFalse },
    { "Chebyshev", ChebyshevKernel, UndefinedOptionFlag, MagickFalse },
    { "Manhattan", ManhattanKernel, UndefinedOptionFlag, MagickFalse },
    { "Octagonal", OctagonalKernel, UndefinedOptionFlag, MagickFalse },
    { "Euclidean", EuclideanKernel, UndefinedOptionFlag, MagickFalse },
    { "User Defined", UserDefinedKernel, UndefinedOptionFlag, MagickTrue },
    { (char *) NULL, UndefinedKernel, UndefinedOptionFlag, MagickFalse }
  },
  LayerOptions[] =
  {
    { "Undefined", UndefinedLayer, UndefinedOptionFlag, MagickTrue },
    { "Coalesce", CoalesceLayer, UndefinedOptionFlag, MagickFalse },
    { "CompareAny", CompareAnyLayer, UndefinedOptionFlag, MagickFalse },
    { "CompareClear", CompareClearLayer, UndefinedOptionFlag, MagickFalse },
    { "CompareOverlay", CompareOverlayLayer, UndefinedOptionFlag, MagickFalse },
    { "Dispose", DisposeLayer, UndefinedOptionFlag, MagickFalse },
    { "Optimize", OptimizeLayer, UndefinedOptionFlag, MagickFalse },
    { "OptimizeFrame", OptimizeImageLayer, UndefinedOptionFlag, MagickFalse },
    { "OptimizePlus", OptimizePlusLayer, UndefinedOptionFlag, MagickFalse },
    { "OptimizeTransparency", OptimizeTransLayer, UndefinedOptionFlag, MagickFalse },
    { "RemoveDups", RemoveDupsLayer, UndefinedOptionFlag, MagickFalse },
    { "RemoveZero", RemoveZeroLayer, UndefinedOptionFlag, MagickFalse },
    { "Composite", CompositeLayer, UndefinedOptionFlag, MagickFalse },
    { "Merge", MergeLayer, UndefinedOptionFlag, MagickFalse },
    { "Flatten", FlattenLayer, UndefinedOptionFlag, MagickFalse },
    { "Mosaic", MosaicLayer, UndefinedOptionFlag, MagickFalse },
    { "TrimBounds", TrimBoundsLayer, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedLayer, UndefinedOptionFlag, MagickFalse }
  },
  LineCapOptions[] =
  {
    { "Undefined", UndefinedCap, UndefinedOptionFlag, MagickTrue },
    { "Butt", ButtCap, UndefinedOptionFlag, MagickFalse },
    { "Round", RoundCap, UndefinedOptionFlag, MagickFalse },
    { "Square", SquareCap, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedCap, UndefinedOptionFlag, MagickFalse }
  },
  LineJoinOptions[] =
  {
    { "Undefined", UndefinedJoin, UndefinedOptionFlag, MagickTrue },
    { "Bevel", BevelJoin, UndefinedOptionFlag, MagickFalse },
    { "Miter", MiterJoin, UndefinedOptionFlag, MagickFalse },
    { "Round", RoundJoin, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedJoin, UndefinedOptionFlag, MagickFalse }
  },
  ListOptions[] =
  {
    { "Align", MagickAlignOptions, UndefinedOptionFlag, MagickFalse },
    { "Alpha", MagickAlphaChannelOptions, UndefinedOptionFlag, MagickFalse },
    { "Boolean", MagickBooleanOptions, UndefinedOptionFlag, MagickFalse },
    { "Cache", MagickCacheOptions, UndefinedOptionFlag, MagickFalse },
    { "Channel", MagickChannelOptions, UndefinedOptionFlag, MagickFalse },
    { "Class", MagickClassOptions, UndefinedOptionFlag, MagickFalse },
    { "ClipPath", MagickClipPathOptions, UndefinedOptionFlag, MagickFalse },
    { "Coder", MagickCoderOptions, UndefinedOptionFlag, MagickFalse },
    { "Color", MagickColorOptions, UndefinedOptionFlag, MagickFalse },
    { "Colorspace", MagickColorspaceOptions, UndefinedOptionFlag, MagickFalse },
    { "Command", MagickCommandOptions, UndefinedOptionFlag, MagickFalse },
    { "Compose", MagickComposeOptions, UndefinedOptionFlag, MagickFalse },
    { "Compress", MagickCompressOptions, UndefinedOptionFlag, MagickFalse },
    { "Configure", MagickConfigureOptions, UndefinedOptionFlag, MagickFalse },
    { "DataType", MagickDataTypeOptions, UndefinedOptionFlag, MagickFalse },
    { "Debug", MagickDebugOptions, UndefinedOptionFlag, MagickFalse },
    { "Decoration", MagickDecorateOptions, UndefinedOptionFlag, MagickFalse },
    { "Delegate", MagickDelegateOptions, UndefinedOptionFlag, MagickFalse },
    { "Direction", MagickDirectionOptions, UndefinedOptionFlag, MagickFalse },
    { "Dispose", MagickDisposeOptions, UndefinedOptionFlag, MagickFalse },
    { "Distort", MagickDistortOptions, UndefinedOptionFlag, MagickFalse },
    { "Dither", MagickDitherOptions, UndefinedOptionFlag, MagickFalse },
    { "Endian", MagickEndianOptions, UndefinedOptionFlag, MagickFalse },
    { "Evaluate", MagickEvaluateOptions, UndefinedOptionFlag, MagickFalse },
    { "FillRule", MagickFillRuleOptions, UndefinedOptionFlag, MagickFalse },
    { "Filter", MagickFilterOptions, UndefinedOptionFlag, MagickFalse },
    { "Font", MagickFontOptions, UndefinedOptionFlag, MagickFalse },
    { "Format", MagickFormatOptions, UndefinedOptionFlag, MagickFalse },
    { "Function", MagickFunctionOptions, UndefinedOptionFlag, MagickFalse },
    { "Gravity", MagickGravityOptions, UndefinedOptionFlag, MagickFalse },
    { "Intensity", MagickPixelIntensityOptions, UndefinedOptionFlag, MagickFalse },
    { "Intent", MagickIntentOptions, UndefinedOptionFlag, MagickFalse },
    { "Interlace", MagickInterlaceOptions, UndefinedOptionFlag, MagickFalse },
    { "Interpolate", MagickInterpolateOptions, UndefinedOptionFlag, MagickFalse },
    { "Kernel", MagickKernelOptions, UndefinedOptionFlag, MagickFalse },
    { "Layers", MagickLayerOptions, UndefinedOptionFlag, MagickFalse },
    { "LineCap", MagickLineCapOptions, UndefinedOptionFlag, MagickFalse },
    { "LineJoin", MagickLineJoinOptions, UndefinedOptionFlag, MagickFalse },
    { "List", MagickListOptions, UndefinedOptionFlag, MagickFalse },
    { "Locale", MagickLocaleOptions, UndefinedOptionFlag, MagickFalse },
    { "LogEvent", MagickLogEventOptions, UndefinedOptionFlag, MagickFalse },
    { "Log", MagickLogOptions, UndefinedOptionFlag, MagickFalse },
    { "Magic", MagickMagicOptions, UndefinedOptionFlag, MagickFalse },
    { "Method", MagickMethodOptions, UndefinedOptionFlag, MagickFalse },
    { "Metric", MagickMetricOptions, UndefinedOptionFlag, MagickFalse },
    { "Mime", MagickMimeOptions, UndefinedOptionFlag, MagickFalse },
    { "Mode", MagickModeOptions, UndefinedOptionFlag, MagickFalse },
    { "Morphology", MagickMorphologyOptions, UndefinedOptionFlag, MagickFalse },
    { "Module", MagickModuleOptions, UndefinedOptionFlag, MagickFalse },
    { "Noise", MagickNoiseOptions, UndefinedOptionFlag, MagickFalse },
    { "Orientation", MagickOrientationOptions, UndefinedOptionFlag, MagickFalse },
    { "PixelChannel", MagickPixelChannelOptions, UndefinedOptionFlag, MagickFalse },
    { "PixelIntensity", MagickPixelIntensityOptions, UndefinedOptionFlag, MagickFalse },
    { "PixelMask", MagickPixelMaskOptions, UndefinedOptionFlag, MagickFalse },
    { "PixelTrait", MagickPixelTraitOptions, UndefinedOptionFlag, MagickFalse },
    { "Policy", MagickPolicyOptions, UndefinedOptionFlag, MagickFalse },
    { "PolicyDomain", MagickPolicyDomainOptions, UndefinedOptionFlag, MagickFalse },
    { "PolicyRights", MagickPolicyRightsOptions, UndefinedOptionFlag, MagickFalse },
    { "Preview", MagickPreviewOptions, UndefinedOptionFlag, MagickFalse },
    { "Primitive", MagickPrimitiveOptions, UndefinedOptionFlag, MagickFalse },
    { "QuantumFormat", MagickQuantumFormatOptions, UndefinedOptionFlag, MagickFalse },
    { "Resource", MagickResourceOptions, UndefinedOptionFlag, MagickFalse },
    { "SparseColor", MagickSparseColorOptions, UndefinedOptionFlag, MagickFalse },
    { "Statistic", MagickStatisticOptions, UndefinedOptionFlag, MagickFalse },
    { "Storage", MagickStorageOptions, UndefinedOptionFlag, MagickFalse },
    { "Stretch", MagickStretchOptions, UndefinedOptionFlag, MagickFalse },
    { "Style", MagickStyleOptions, UndefinedOptionFlag, MagickFalse },
    { "Threshold", MagickThresholdOptions, UndefinedOptionFlag, MagickFalse },
    { "Type", MagickTypeOptions, UndefinedOptionFlag, MagickFalse },
    { "Units", MagickResolutionOptions, UndefinedOptionFlag, MagickFalse },
    { "Undefined", MagickUndefinedOptions, UndefinedOptionFlag, MagickTrue },
    { "Validate", MagickValidateOptions, UndefinedOptionFlag, MagickFalse },
    { "VirtualPixel", MagickVirtualPixelOptions, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, MagickUndefinedOptions, UndefinedOptionFlag, MagickFalse }
  },
  LogEventOptions[] =
  {
    { "Undefined", UndefinedEvents, UndefinedOptionFlag, MagickTrue },
    { "All", (AllEvents &~ TraceEvent), UndefinedOptionFlag, MagickFalse },
    { "Accelerate", AccelerateEvent, UndefinedOptionFlag, MagickFalse },
    { "Annotate", AnnotateEvent, UndefinedOptionFlag, MagickFalse },
    { "Blob", BlobEvent, UndefinedOptionFlag, MagickFalse },
    { "Cache", CacheEvent, UndefinedOptionFlag, MagickFalse },
    { "Coder", CoderEvent, UndefinedOptionFlag, MagickFalse },
    { "Configure", ConfigureEvent, UndefinedOptionFlag, MagickFalse },
    { "Deprecate", DeprecateEvent, UndefinedOptionFlag, MagickFalse },
    { "Draw", DrawEvent, UndefinedOptionFlag, MagickFalse },
    { "Exception", ExceptionEvent, UndefinedOptionFlag, MagickFalse },
    { "Locale", LocaleEvent, UndefinedOptionFlag, MagickFalse },
    { "Module", ModuleEvent, UndefinedOptionFlag, MagickFalse },
    { "None", NoEvents, UndefinedOptionFlag, MagickFalse },
    { "Pixel", PixelEvent, UndefinedOptionFlag, MagickFalse },
    { "Policy", PolicyEvent, UndefinedOptionFlag, MagickFalse },
    { "Resource", ResourceEvent, UndefinedOptionFlag, MagickFalse },
    { "Trace", TraceEvent, UndefinedOptionFlag, MagickFalse },
    { "Transform", TransformEvent, UndefinedOptionFlag, MagickFalse },
    { "User", UserEvent, UndefinedOptionFlag, MagickFalse },
    { "Wand", WandEvent, UndefinedOptionFlag, MagickFalse },
    { "X11", X11Event, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedEvents, UndefinedOptionFlag, MagickFalse }
  },
  MetricOptions[] =
  {
    { "Undefined", UndefinedMetric, UndefinedOptionFlag, MagickTrue },
    { "AE", AbsoluteErrorMetric, UndefinedOptionFlag, MagickFalse },
    { "Fuzz", FuzzErrorMetric, UndefinedOptionFlag, MagickFalse },
    { "MAE", MeanAbsoluteErrorMetric, UndefinedOptionFlag, MagickFalse },
    { "MEPP", MeanErrorPerPixelMetric, UndefinedOptionFlag, MagickFalse },
    { "MSE", MeanSquaredErrorMetric, UndefinedOptionFlag, MagickFalse },
    { "NCC", NormalizedCrossCorrelationErrorMetric, UndefinedOptionFlag, MagickFalse },
    { "PAE", PeakAbsoluteErrorMetric, UndefinedOptionFlag, MagickFalse },
    { "PSNR", PeakSignalToNoiseRatioMetric, UndefinedOptionFlag, MagickFalse },
    { "RMSE", RootMeanSquaredErrorMetric, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedMetric, UndefinedOptionFlag, MagickFalse }
  },
  MethodOptions[] =
  {
    { "Undefined", UndefinedMethod, UndefinedOptionFlag, MagickTrue },
    { "FillToBorder", FillToBorderMethod, UndefinedOptionFlag, MagickFalse },
    { "Floodfill", FloodfillMethod, UndefinedOptionFlag, MagickFalse },
    { "Point", PointMethod, UndefinedOptionFlag, MagickFalse },
    { "Replace", ReplaceMethod, UndefinedOptionFlag, MagickFalse },
    { "Reset", ResetMethod, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedMethod, UndefinedOptionFlag, MagickFalse }
  },
  ModeOptions[] =
  {
    { "Undefined", UndefinedMode, UndefinedOptionFlag, MagickTrue },
    { "Concatenate", ConcatenateMode, UndefinedOptionFlag, MagickFalse },
    { "Frame", FrameMode, UndefinedOptionFlag, MagickFalse },
    { "Unframe", UnframeMode, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedMode, UndefinedOptionFlag, MagickFalse }
  },
  MorphologyOptions[] =
  {
    { "Undefined", UndefinedMorphology, UndefinedOptionFlag, MagickTrue },
    { "Correlate", CorrelateMorphology, UndefinedOptionFlag, MagickFalse },
    { "Convolve", ConvolveMorphology, UndefinedOptionFlag, MagickFalse },
    { "Dilate", DilateMorphology, UndefinedOptionFlag, MagickFalse },
    { "Erode", ErodeMorphology, UndefinedOptionFlag, MagickFalse },
    { "Close", CloseMorphology, UndefinedOptionFlag, MagickFalse },
    { "Open", OpenMorphology, UndefinedOptionFlag, MagickFalse },
    { "DilateIntensity", DilateIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "ErodeIntensity", ErodeIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "CloseIntensity", CloseIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "OpenIntensity", OpenIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "DilateI", DilateIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "ErodeI", ErodeIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "CloseI", CloseIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "OpenI", OpenIntensityMorphology, UndefinedOptionFlag, MagickFalse },
    { "Smooth", SmoothMorphology, UndefinedOptionFlag, MagickFalse },
    { "EdgeOut", EdgeOutMorphology, UndefinedOptionFlag, MagickFalse },
    { "EdgeIn", EdgeInMorphology, UndefinedOptionFlag, MagickFalse },
    { "Edge", EdgeMorphology, UndefinedOptionFlag, MagickFalse },
    { "TopHat", TopHatMorphology, UndefinedOptionFlag, MagickFalse },
    { "BottomHat", BottomHatMorphology, UndefinedOptionFlag, MagickFalse },
    { "Hmt", HitAndMissMorphology, UndefinedOptionFlag, MagickFalse },
    { "HitNMiss", HitAndMissMorphology, UndefinedOptionFlag, MagickFalse },
    { "HitAndMiss", HitAndMissMorphology, UndefinedOptionFlag, MagickFalse },
    { "Thinning", ThinningMorphology, UndefinedOptionFlag, MagickFalse },
    { "Thicken", ThickenMorphology, UndefinedOptionFlag, MagickFalse },
    { "Distance", DistanceMorphology, UndefinedOptionFlag, MagickFalse },
    { "IterativeDistance", IterativeDistanceMorphology, UndefinedOptionFlag, MagickFalse },
    { "Voronoi", VoronoiMorphology, UndefinedOptionFlag, MagickTrue },
    { (char *) NULL, UndefinedMorphology, UndefinedOptionFlag, MagickFalse }
  },
  NoiseOptions[] =
  {
    { "Undefined", UndefinedNoise, UndefinedOptionFlag, MagickTrue },
    { "Gaussian", GaussianNoise, UndefinedOptionFlag, MagickFalse },
    { "Impulse", ImpulseNoise, UndefinedOptionFlag, MagickFalse },
    { "Laplacian", LaplacianNoise, UndefinedOptionFlag, MagickFalse },
    { "Multiplicative", MultiplicativeGaussianNoise, UndefinedOptionFlag, MagickFalse },
    { "Poisson", PoissonNoise, UndefinedOptionFlag, MagickFalse },
    { "Random", RandomNoise, UndefinedOptionFlag, MagickFalse },
    { "Uniform", UniformNoise, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedNoise, UndefinedOptionFlag, MagickFalse }
  },
  OrientationOptions[] =
  {
    { "Undefined", UndefinedOrientation, UndefinedOptionFlag, MagickTrue },
    { "TopLeft", TopLeftOrientation, UndefinedOptionFlag, MagickFalse },
    { "TopRight", TopRightOrientation, UndefinedOptionFlag, MagickFalse },
    { "BottomRight", BottomRightOrientation, UndefinedOptionFlag, MagickFalse },
    { "BottomLeft", BottomLeftOrientation, UndefinedOptionFlag, MagickFalse },
    { "LeftTop", LeftTopOrientation, UndefinedOptionFlag, MagickFalse },
    { "RightTop", RightTopOrientation, UndefinedOptionFlag, MagickFalse },
    { "RightBottom", RightBottomOrientation, UndefinedOptionFlag, MagickFalse },
    { "LeftBottom", LeftBottomOrientation, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedOrientation, UndefinedOptionFlag, MagickFalse }
  },
  PixelChannelOptions[] =
  {
    { "Undefined", UndefinedPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "A", AlphaPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Alpha", AlphaPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "B", BluePixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Bk", BlackPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Black", BlackPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Blue", BluePixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Cb", CbPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Composite", CompositePixelChannel, UndefinedOptionFlag, MagickFalse },
    { "C", CyanPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Cr", CrPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Cyan", CyanPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Gray", GrayPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "G", GreenPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Green", GreenPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Index", IndexPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Intensity", IntensityPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "K", BlackPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "M", MagentaPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Magenta", MagentaPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Mask", MaskPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "R", RedPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Red", RedPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Sync", SyncPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Y", YellowPixelChannel, UndefinedOptionFlag, MagickFalse },
    { "Yellow", YellowPixelChannel, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPixelChannel, UndefinedOptionFlag, MagickFalse }
  },
  PixelIntensityOptions[] =
  {
    { "Undefined", UndefinedPixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Average", AveragePixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Brightness", BrightnessPixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Lightness", LightnessPixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Rec601Luma", Rec601LumaPixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Rec601Luminance", Rec601LuminancePixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Rec709Luma", Rec709LumaPixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "Rec709Luminance", Rec709LuminancePixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { "RMS", RMSPixelIntensityMethod, UndefinedOptionFlag, MagickTrue },
    { (char *) NULL, UndefinedPixelIntensityMethod, UndefinedOptionFlag, MagickFalse }
  },
  PixelMaskOptions[] =
  {
    { "Undefined", UndefinedPixelMask, UndefinedOptionFlag, MagickTrue },
    { "R", ReadPixelMask, UndefinedOptionFlag, MagickFalse },
    { "Read", ReadPixelMask, UndefinedOptionFlag, MagickFalse },
    { "W", WritePixelMask, UndefinedOptionFlag, MagickFalse },
    { "Write", WritePixelMask, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPixelMask, UndefinedOptionFlag, MagickFalse }
  },
  PixelTraitOptions[] =
  {
    { "Undefined", UndefinedPixelTrait, UndefinedOptionFlag, MagickTrue },
    { "Blend", BlendPixelTrait, UndefinedOptionFlag, MagickFalse },
    { "Copy", CopyPixelTrait, UndefinedOptionFlag, MagickFalse },
    { "Update", UpdatePixelTrait, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPixelTrait, UndefinedOptionFlag, MagickFalse }
  },
  PolicyDomainOptions[] =
  {
    { "Undefined", UndefinedPolicyDomain, UndefinedOptionFlag, MagickTrue },
    { "Coder", CoderPolicyDomain, UndefinedOptionFlag, MagickFalse },
    { "Delegate", DelegatePolicyDomain, UndefinedOptionFlag, MagickFalse },
    { "Filter", FilterPolicyDomain, UndefinedOptionFlag, MagickFalse },
    { "Path", PathPolicyDomain, UndefinedOptionFlag, MagickFalse },
    { "Resource", ResourcePolicyDomain, UndefinedOptionFlag, MagickFalse },
    { "System", SystemPolicyDomain, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPolicyDomain, UndefinedOptionFlag, MagickFalse }
  },
  PolicyRightsOptions[] =
  {
    { "Undefined", UndefinedPolicyRights, UndefinedOptionFlag, MagickTrue },
    { "None", NoPolicyRights, UndefinedOptionFlag, MagickFalse },
    { "Read", ReadPolicyRights, UndefinedOptionFlag, MagickFalse },
    { "Write", WritePolicyRights, UndefinedOptionFlag, MagickFalse },
    { "Execute", ExecutePolicyRights, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPolicyRights, UndefinedOptionFlag, MagickFalse }
  },
  PreviewOptions[] =
  {
    { "Undefined", UndefinedPreview, UndefinedOptionFlag, MagickTrue },
    { "AddNoise", AddNoisePreview, UndefinedOptionFlag, MagickFalse },
    { "Blur", BlurPreview, UndefinedOptionFlag, MagickFalse },
    { "Brightness", BrightnessPreview, UndefinedOptionFlag, MagickFalse },
    { "Charcoal", CharcoalDrawingPreview, UndefinedOptionFlag, MagickFalse },
    { "Despeckle", DespecklePreview, UndefinedOptionFlag, MagickFalse },
    { "Dull", DullPreview, UndefinedOptionFlag, MagickFalse },
    { "EdgeDetect", EdgeDetectPreview, UndefinedOptionFlag, MagickFalse },
    { "Gamma", GammaPreview, UndefinedOptionFlag, MagickFalse },
    { "Grayscale", GrayscalePreview, UndefinedOptionFlag, MagickFalse },
    { "Hue", HuePreview, UndefinedOptionFlag, MagickFalse },
    { "Implode", ImplodePreview, UndefinedOptionFlag, MagickFalse },
    { "JPEG", JPEGPreview, UndefinedOptionFlag, MagickFalse },
    { "OilPaint", OilPaintPreview, UndefinedOptionFlag, MagickFalse },
    { "Quantize", QuantizePreview, UndefinedOptionFlag, MagickFalse },
    { "Raise", RaisePreview, UndefinedOptionFlag, MagickFalse },
    { "ReduceNoise", ReduceNoisePreview, UndefinedOptionFlag, MagickFalse },
    { "Roll", RollPreview, UndefinedOptionFlag, MagickFalse },
    { "Rotate", RotatePreview, UndefinedOptionFlag, MagickFalse },
    { "Saturation", SaturationPreview, UndefinedOptionFlag, MagickFalse },
    { "Segment", SegmentPreview, UndefinedOptionFlag, MagickFalse },
    { "Shade", ShadePreview, UndefinedOptionFlag, MagickFalse },
    { "Sharpen", SharpenPreview, UndefinedOptionFlag, MagickFalse },
    { "Shear", ShearPreview, UndefinedOptionFlag, MagickFalse },
    { "Solarize", SolarizePreview, UndefinedOptionFlag, MagickFalse },
    { "Spiff", SpiffPreview, UndefinedOptionFlag, MagickFalse },
    { "Spread", SpreadPreview, UndefinedOptionFlag, MagickFalse },
    { "Swirl", SwirlPreview, UndefinedOptionFlag, MagickFalse },
    { "Threshold", ThresholdPreview, UndefinedOptionFlag, MagickFalse },
    { "Wave", WavePreview, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPreview, UndefinedOptionFlag, MagickFalse }
  },
  PrimitiveOptions[] =
  {
    { "Undefined", UndefinedPrimitive, UndefinedOptionFlag, MagickTrue },
    { "Arc", ArcPrimitive, UndefinedOptionFlag, MagickFalse },
    { "Bezier", BezierPrimitive, UndefinedOptionFlag, MagickFalse },
    { "Circle", CirclePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Color", ColorPrimitive, UndefinedOptionFlag, MagickFalse },
    { "Ellipse", EllipsePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Image", ImagePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Line", LinePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Matte", MattePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Path", PathPrimitive, UndefinedOptionFlag, MagickFalse },
    { "Point", PointPrimitive, UndefinedOptionFlag, MagickFalse },
    { "Polygon", PolygonPrimitive, UndefinedOptionFlag, MagickFalse },
    { "Polyline", PolylinePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Rectangle", RectanglePrimitive, UndefinedOptionFlag, MagickFalse },
    { "RoundRectangle", RoundRectanglePrimitive, UndefinedOptionFlag, MagickFalse },
    { "Text", TextPrimitive, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedPrimitive, UndefinedOptionFlag, MagickFalse }
  },
  QuantumFormatOptions[] =
  {
    { "Undefined", UndefinedQuantumFormat, UndefinedOptionFlag, MagickTrue },
    { "FloatingPoint", FloatingPointQuantumFormat, UndefinedOptionFlag, MagickFalse },
    { "Signed", SignedQuantumFormat, UndefinedOptionFlag, MagickFalse },
    { "Unsigned", UnsignedQuantumFormat, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, FloatingPointQuantumFormat, UndefinedOptionFlag, MagickFalse }
  },
  ResolutionOptions[] =
  {
    { "Undefined", UndefinedResolution, UndefinedOptionFlag, MagickTrue },
    { "PixelsPerInch", PixelsPerInchResolution, UndefinedOptionFlag, MagickFalse },
    { "PixelsPerCentimeter", PixelsPerCentimeterResolution, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedResolution, UndefinedOptionFlag, MagickFalse }
  },
  ResourceOptions[] =
  {
    { "Undefined", UndefinedResource, UndefinedOptionFlag, MagickTrue },
    { "Area", AreaResource, UndefinedOptionFlag, MagickFalse },
    { "Disk", DiskResource, UndefinedOptionFlag, MagickFalse },
    { "File", FileResource, UndefinedOptionFlag, MagickFalse },
    { "Map", MapResource, UndefinedOptionFlag, MagickFalse },
    { "Memory", MemoryResource, UndefinedOptionFlag, MagickFalse },
    { "Thread", ThreadResource, UndefinedOptionFlag, MagickFalse },
    { "Time", TimeResource, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedResource, UndefinedOptionFlag, MagickFalse }
  },
  SparseColorOptions[] =
  {
    { "Undefined", UndefinedDistortion, UndefinedOptionFlag, MagickTrue },
    { "Barycentric", BarycentricColorInterpolate, UndefinedOptionFlag, MagickFalse },
    { "Bilinear", BilinearColorInterpolate, UndefinedOptionFlag, MagickFalse },
    { "Inverse", InverseColorInterpolate, UndefinedOptionFlag, MagickFalse },
    { "Shepards", ShepardsColorInterpolate, UndefinedOptionFlag, MagickFalse },
    { "Voronoi", VoronoiColorInterpolate, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedResource, UndefinedOptionFlag, MagickFalse }
  },
  StatisticOptions[] =
  {
    { "Undefined", UndefinedStatistic, UndefinedOptionFlag, MagickTrue },
    { "Gradient", GradientStatistic, UndefinedOptionFlag, MagickFalse },
    { "Maximum", MaximumStatistic, UndefinedOptionFlag, MagickFalse },
    { "Mean", MeanStatistic, UndefinedOptionFlag, MagickFalse },
    { "Median", MedianStatistic, UndefinedOptionFlag, MagickFalse },
    { "Minimum", MinimumStatistic, UndefinedOptionFlag, MagickFalse },
    { "Mode", ModeStatistic, UndefinedOptionFlag, MagickFalse },
    { "NonPeak", NonpeakStatistic, UndefinedOptionFlag, MagickFalse },
    { "StandardDeviation", StandardDeviationStatistic, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedMethod, UndefinedOptionFlag, MagickFalse }
  },
  StorageOptions[] =
  {
    { "Undefined", UndefinedPixel, UndefinedOptionFlag, MagickTrue },
    { "Char", CharPixel, UndefinedOptionFlag, MagickFalse },
    { "Double", DoublePixel, UndefinedOptionFlag, MagickFalse },
    { "Float", FloatPixel, UndefinedOptionFlag, MagickFalse },
    { "Long", LongPixel, UndefinedOptionFlag, MagickFalse },
    { "LongLong", LongLongPixel, UndefinedOptionFlag, MagickFalse },
    { "Quantum", QuantumPixel, UndefinedOptionFlag, MagickFalse },
    { "Short", ShortPixel, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedResource, UndefinedOptionFlag, MagickFalse }
  },
  StretchOptions[] =
  {
    { "Undefined", UndefinedStretch, UndefinedOptionFlag, MagickTrue },
    { "Any", AnyStretch, UndefinedOptionFlag, MagickFalse },
    { "Condensed", CondensedStretch, UndefinedOptionFlag, MagickFalse },
    { "Expanded", ExpandedStretch, UndefinedOptionFlag, MagickFalse },
    { "ExtraCondensed", ExtraCondensedStretch, UndefinedOptionFlag, MagickFalse },
    { "ExtraExpanded", ExtraExpandedStretch, UndefinedOptionFlag, MagickFalse },
    { "Normal", NormalStretch, UndefinedOptionFlag, MagickFalse },
    { "SemiCondensed", SemiCondensedStretch, UndefinedOptionFlag, MagickFalse },
    { "SemiExpanded", SemiExpandedStretch, UndefinedOptionFlag, MagickFalse },
    { "UltraCondensed", UltraCondensedStretch, UndefinedOptionFlag, MagickFalse },
    { "UltraExpanded", UltraExpandedStretch, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedStretch, UndefinedOptionFlag, MagickFalse }
  },
  StyleOptions[] =
  {
    { "Undefined", UndefinedStyle, UndefinedOptionFlag, MagickTrue },
    { "Any", AnyStyle, UndefinedOptionFlag, MagickFalse },
    { "Italic", ItalicStyle, UndefinedOptionFlag, MagickFalse },
    { "Normal", NormalStyle, UndefinedOptionFlag, MagickFalse },
    { "Oblique", ObliqueStyle, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedStyle, UndefinedOptionFlag, MagickFalse }
  },
  TypeOptions[] =
  {
    { "Undefined", UndefinedType, UndefinedOptionFlag, MagickTrue },
    { "Bilevel", BilevelType, UndefinedOptionFlag, MagickFalse },
    { "ColorSeparation", ColorSeparationType, UndefinedOptionFlag, MagickFalse },
    { "ColorSeparationAlpha", ColorSeparationMatteType, UndefinedOptionFlag, MagickFalse },
    { "ColorSeparationMatte", ColorSeparationMatteType, UndefinedOptionFlag, MagickFalse },
    { "Grayscale", GrayscaleType, UndefinedOptionFlag, MagickFalse },
    { "GrayscaleAlpha", GrayscaleMatteType, UndefinedOptionFlag, MagickFalse },
    { "GrayscaleMatte", GrayscaleMatteType, UndefinedOptionFlag, MagickFalse },
    { "Optimize", OptimizeType, UndefinedOptionFlag, MagickFalse },
    { "Palette", PaletteType, UndefinedOptionFlag, MagickFalse },
    { "PaletteBilevelAlpha", PaletteBilevelMatteType, UndefinedOptionFlag, MagickFalse },
    { "PaletteBilevelMatte", PaletteBilevelMatteType, UndefinedOptionFlag, MagickFalse },
    { "PaletteAlpha", PaletteMatteType, UndefinedOptionFlag, MagickFalse },
    { "PaletteMatte", PaletteMatteType, UndefinedOptionFlag, MagickFalse },
    { "TrueColorAlpha", TrueColorMatteType, UndefinedOptionFlag, MagickFalse },
    { "TrueColorMatte", TrueColorMatteType, UndefinedOptionFlag, MagickFalse },
    { "TrueColor", TrueColorType, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedType, UndefinedOptionFlag, MagickFalse }
  },
  ValidateOptions[] =
  {
    { "Undefined", UndefinedValidate, UndefinedOptionFlag, MagickTrue },
    { "All", AllValidate, UndefinedOptionFlag, MagickFalse },
    { "Compare", CompareValidate, UndefinedOptionFlag, MagickFalse },
    { "Composite", CompositeValidate, UndefinedOptionFlag, MagickFalse },
    { "Convert", ConvertValidate, UndefinedOptionFlag, MagickFalse },
    { "FormatsInMemory", FormatsInMemoryValidate, UndefinedOptionFlag, MagickFalse },
    { "FormatsOnDisk", FormatsOnDiskValidate, UndefinedOptionFlag, MagickFalse },
    { "Identify", IdentifyValidate, UndefinedOptionFlag, MagickFalse },
    { "ImportExport", ImportExportValidate, UndefinedOptionFlag, MagickFalse },
    { "Montage", MontageValidate, UndefinedOptionFlag, MagickFalse },
    { "Stream", StreamValidate, UndefinedOptionFlag, MagickFalse },
    { "None", NoValidate, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedValidate, UndefinedOptionFlag, MagickFalse }
  },
  VirtualPixelOptions[] =
  {
    { "Undefined", UndefinedVirtualPixelMethod, UndefinedOptionFlag, MagickTrue },
    { "Background", BackgroundVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Black", BlackVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Constant", BackgroundVirtualPixelMethod, DeprecateOptionFlag, MagickTrue },
    { "CheckerTile", CheckerTileVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Dither", DitherVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Edge", EdgeVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Gray", GrayVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "HorizontalTile", HorizontalTileVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "HorizontalTileEdge", HorizontalTileEdgeVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Mirror", MirrorVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "None", TransparentVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Random", RandomVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Tile", TileVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "Transparent", TransparentVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "VerticalTile", VerticalTileVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "VerticalTileEdge", VerticalTileEdgeVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { "White", WhiteVirtualPixelMethod, UndefinedOptionFlag, MagickFalse },
    { (char *) NULL, UndefinedVirtualPixelMethod, UndefinedOptionFlag, MagickFalse }
  };

static const OptionInfo *GetOptionInfo(const CommandOption option)
{
  switch (option)
  {
    case MagickAlignOptions: return(AlignOptions);
    case MagickAlphaChannelOptions: return(AlphaChannelOptions);
    case MagickBooleanOptions: return(BooleanOptions);
    case MagickCacheOptions: return(CacheOptions);
    case MagickChannelOptions: return(ChannelOptions);
    case MagickClassOptions: return(ClassOptions);
    case MagickClipPathOptions: return(ClipPathOptions);
    case MagickColorspaceOptions: return(ColorspaceOptions);
    case MagickCommandOptions: return(CommandOptions);
    case MagickComposeOptions: return(ComposeOptions);
    case MagickCompressOptions: return(CompressOptions);
    case MagickDataTypeOptions: return(DataTypeOptions);
    case MagickDebugOptions: return(LogEventOptions);
    case MagickDecorateOptions: return(DecorateOptions);
    case MagickDirectionOptions: return(DirectionOptions);
    case MagickDisposeOptions: return(DisposeOptions);
    case MagickDistortOptions: return(DistortOptions);
    case MagickDitherOptions: return(DitherOptions);
    case MagickEndianOptions: return(EndianOptions);
    case MagickEvaluateOptions: return(EvaluateOptions);
    case MagickFillRuleOptions: return(FillRuleOptions);
    case MagickFilterOptions: return(FilterOptions);
    case MagickFunctionOptions: return(FunctionOptions);
    case MagickGravityOptions: return(GravityOptions);
/*  case MagickImageListOptions: return(ImageListOptions); */
    case MagickIntentOptions: return(IntentOptions);
    case MagickInterlaceOptions: return(InterlaceOptions);
    case MagickInterpolateOptions: return(InterpolateOptions);
    case MagickKernelOptions: return(KernelOptions);
    case MagickLayerOptions: return(LayerOptions);
    case MagickLineCapOptions: return(LineCapOptions);
    case MagickLineJoinOptions: return(LineJoinOptions);
    case MagickListOptions: return(ListOptions);
    case MagickLogEventOptions: return(LogEventOptions);
    case MagickMetricOptions: return(MetricOptions);
    case MagickMethodOptions: return(MethodOptions);
    case MagickModeOptions: return(ModeOptions);
    case MagickMorphologyOptions: return(MorphologyOptions);
    case MagickNoiseOptions: return(NoiseOptions);
    case MagickOrientationOptions: return(OrientationOptions);
    case MagickPixelChannelOptions: return(PixelChannelOptions);
    case MagickPixelIntensityOptions: return(PixelIntensityOptions);
    case MagickPixelMaskOptions: return(PixelMaskOptions);
    case MagickPixelTraitOptions: return(PixelTraitOptions);
    case MagickPolicyDomainOptions: return(PolicyDomainOptions);
    case MagickPolicyRightsOptions: return(PolicyRightsOptions);
    case MagickPreviewOptions: return(PreviewOptions);
    case MagickPrimitiveOptions: return(PrimitiveOptions);
    case MagickQuantumFormatOptions: return(QuantumFormatOptions);
    case MagickResolutionOptions: return(ResolutionOptions);
    case MagickResourceOptions: return(ResourceOptions);
    case MagickSparseColorOptions: return(SparseColorOptions);
    case MagickStatisticOptions: return(StatisticOptions);
    case MagickStorageOptions: return(StorageOptions);
    case MagickStretchOptions: return(StretchOptions);
    case MagickStyleOptions: return(StyleOptions);
    case MagickTypeOptions: return(TypeOptions);
    case MagickValidateOptions: return(ValidateOptions);
    case MagickVirtualPixelOptions: return(VirtualPixelOptions);
    default: break;
  }
  return((const OptionInfo *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e O p t i o n s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageOptions() clones all global image options, to another image_info
%
%  The format of the CloneImageOptions method is:
%
%      MagickBooleanType CloneImageOptions(ImageInfo *image_info,
%        const ImageInfo *clone_info)
%
%  A description of each parameter follows:
%
%    o image_info: the image info to recieve the cloned options.
%
%    o clone_info: the source image info for options to clone.
%
*/
MagickExport MagickBooleanType CloneImageOptions(ImageInfo *image_info,
  const ImageInfo *clone_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(clone_info != (const ImageInfo *) NULL);
  assert(clone_info->signature == MagickSignature);
  if (clone_info->options != (void *) NULL)
    image_info->options=CloneSplayTree((SplayTreeInfo *) clone_info->options,
      (void *(*)(void *)) ConstantString,(void *(*)(void *)) ConstantString);
  return(MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e f i n e I m a g e O p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DefineImageOption() associates an assignment string of the form
%  "key=value" with a global image option. It is equivelent to
%  SetImageOption().
%
%  The format of the DefineImageOption method is:
%
%      MagickBooleanType DefineImageOption(ImageInfo *image_info,
%        const char *option)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o option: the image option assignment string.
%
*/
MagickExport MagickBooleanType DefineImageOption(ImageInfo *image_info,
  const char *option)
{
  char
    key[MaxTextExtent],
    value[MaxTextExtent];

  register char
    *p;

  assert(image_info != (ImageInfo *) NULL);
  assert(option != (const char *) NULL);
  (void) CopyMagickString(key,option,MaxTextExtent);
  for (p=key; *p != '\0'; p++)
    if (*p == '=')
      break;
  *value='\0';
  if (*p == '=')
    (void) CopyMagickString(value,p+1,MaxTextExtent);
  *p='\0';
  return(SetImageOption(image_info,key,value));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l e t e I m a g e O p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageOption() deletes an key from the global image options.
%
%  Returns MagickTrue is the option is found and deleted from the Options.
%
%  The format of the DeleteImageOption method is:
%
%      MagickBooleanType DeleteImageOption(ImageInfo *image_info,
%        const char *key)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o option: the image option.
%
*/
MagickExport MagickBooleanType DeleteImageOption(ImageInfo *image_info,
  const char *option)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return(MagickFalse);
  return(DeleteNodeFromSplayTree((SplayTreeInfo *) image_info->options,option));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e O p t i o n s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageOptions() destroys all global options and associated memory
%  attached to the given image_info image list.
%
%  The format of the DestroyDefines method is:
%
%      void DestroyImageOptions(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
*/
MagickExport void DestroyImageOptions(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options != (void *) NULL)
    image_info->options=DestroySplayTree((SplayTreeInfo *) image_info->options);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e O p t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageOption() gets a value associated with the global image options.
%
%  The returned string is a constant string in the tree and should NOT be
%  freed by the caller.
%
%  The format of the GetImageOption method is:
%
%      const char *GetImageOption(const ImageInfo *image_info,
%        const char *option)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o option: the option.
%
*/
MagickExport const char *GetImageOption(const ImageInfo *image_info,
  const char *option)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return((const char *) NULL);
  return((const char *) GetValueFromSplayTree((SplayTreeInfo *)
    image_info->options,option));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o m m a n d O p t i o n F l a g s                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetCommandOptionFlags() parses a string and returns an enumerated option
%  flags(s).  Return a value of -1 if no such option is found.
%
%  The format of the GetCommandOptionFlags method is:
%
%      ssize_t GetCommandOptionFlags(const CommandOption option,
%        const MagickBooleanType list,const char *options)
%
%  A description of each parameter follows:
%
%    o option: Index to the option table to lookup
%
%    o list: A option other than zero permits more than one option separated by
%      a comma or pipe.
%
%    o options: One or more options separated by commas.
%
*/

MagickExport ssize_t GetCommandOptionFlags(const CommandOption option,
  const MagickBooleanType list,const char *options)
{
  char
    token[MaxTextExtent];

  const OptionInfo
    *option_info;

  int
    sentinel;

  MagickBooleanType
    negate;

  register char
    *q;

  register const char
    *p;

  register ssize_t
    i;

  ssize_t
    option_types;

  option_info=GetOptionInfo(option);
  if (option_info == (const OptionInfo *) NULL)
    return(UndefinedOptionFlag);
  option_types=0;
  sentinel=',';
  if (strchr(options,'|') != (char *) NULL)
    sentinel='|';
  for (p=options; p != (char *) NULL; p=strchr(p,sentinel))
  {
    while (((isspace((int) ((unsigned char) *p)) != 0) || (*p == sentinel)) &&
           (*p != '\0'))
      p++;
    negate=(*p == '!') ? MagickTrue : MagickFalse;
    if (negate != MagickFalse)
      p++;
    q=token;
    while (((isspace((int) ((unsigned char) *p)) == 0) && (*p != sentinel)) &&
           (*p != '\0'))
    {
      if ((q-token) >= (MaxTextExtent-1))
        break;
      *q++=(*p++);
    }
    *q='\0';
    for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
      if (LocaleCompare(token,option_info[i].mnemonic) == 0)
        {
          if (*token == '!')
            option_types=option_types &~ option_info[i].flags;
          else
            option_types=option_types | option_info[i].flags;
          break;
        }
    if ((option_info[i].mnemonic == (char *) NULL) &&
        ((strchr(token+1,'-') != (char *) NULL) ||
         (strchr(token+1,'_') != (char *) NULL)))
      {
        while ((q=strchr(token+1,'-')) != (char *) NULL)
          (void) CopyMagickString(q,q+1,MaxTextExtent-strlen(q));
        while ((q=strchr(token+1,'_')) != (char *) NULL)
          (void) CopyMagickString(q,q+1,MaxTextExtent-strlen(q));
        for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
          if (LocaleCompare(token,option_info[i].mnemonic) == 0)
            {
              if (*token == '!')
                option_types=option_types &~ option_info[i].flags;
              else
                option_types=option_types | option_info[i].flags;
              break;
            }
      }
    if (option_info[i].mnemonic == (char *) NULL)
      return(-1);
    if (list == MagickFalse)
      break;
  }
  return(option_types);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o m m a n d O p t i o n I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetCommandOptionInfo() returns pointer to the matching OptionInfo entry
%  for the "CommandOptions" table only. A specialised binary search is used,
%  to speed up the lookup for that very large table, and returns both the
%  type (arg count) and flags (arg type).
%
%  This search reduces linear search of over 500 options (250 tests on
%  average) to about 10 lookups!
%
%  The format of the GetCommandOptionInfo method is:
%
%      const char **GetCommandOptions(const CommandOption value)
%
%  A description of each parameter follows:
%
%    o value: the value.
%
*/
MagickExport const OptionInfo *GetCommandOptionInfo(const char *value)
{
  const OptionInfo
    *option_info=CommandOptions;

  static ssize_t
    table_size = 0;

  register int
    i,l,h;

  assert(value != (char *) NULL);
  assert(*value != '\0');

  /* count up table items - first time only */
  if ( table_size == 0 )
    {
      l=-1;
      for (i=0; option_info[i].mnemonic != (const char *) NULL; i++)
        if ( LocaleCompare(value,option_info[i].mnemonic) == 0 )
          l=i;
      table_size = i;
      return( &option_info[(l>=0)?l:i] );
    }

  /* faster binary search of command table, now that its length is known */
  l=0;
  h=table_size;
  while ( l < h )
  {
    int cmp;
    i = (l+h)/2; /* half the bounds */
    /* compare string part, then switch character! */
    cmp=LocaleCompare(value+1,option_info[i].mnemonic+1);
    if ( cmp == 0 )
      cmp = *value - *(option_info[i].mnemonic);
#if 0
    (void) FormatLocaleFile(stderr,
      "%d --- %u < %u < %u --- \"%s\" < \"%s\" < \"%s\"\n",
      cmp,l,i,h,option_info[l].mnemonic,option_info[i].mnemonic,
      option_info[h].mnemonic);
#endif
    if (cmp == 0)
      return(&option_info[i]);
    if (cmp > 0) l=i+1; else h=i;  /* reassign search bounds */
  }
  /* option was not found in table - return last 'null' entry. */
  return(&option_info[table_size]);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o m m a n d O p t i o n s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetCommandOptions() returns a list of values.
%
%  The format of the GetCommandOptions method is:
%
%      const char **GetCommandOptions(const CommandOption value)
%
%  A description of each parameter follows:
%
%    o value: the value.
%
*/
MagickExport char **GetCommandOptions(const CommandOption value)
{
  char
    **values;

  const OptionInfo
    *option_info;

  register ssize_t
    i;

  option_info=GetOptionInfo(value);
  if (option_info == (const OptionInfo *) NULL)
    return((char **) NULL);
  for (i=0; option_info[i].mnemonic != (const char *) NULL; i++) ;
  values=(char **) AcquireQuantumMemory((size_t) i+1UL,sizeof(*values));
  if (values == (char **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  for (i=0; option_info[i].mnemonic != (const char *) NULL; i++)
    values[i]=AcquireString(option_info[i].mnemonic);
  values[i]=(char *) NULL;
  return(values);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N e x t I m a g e O p t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNextImageOption() gets the next global option value.
%
%  The format of the GetNextImageOption method is:
%
%      char *GetNextImageOption(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
*/
MagickExport char *GetNextImageOption(const ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return((char *) NULL);
  return((char *) GetNextKeyInSplayTree((SplayTreeInfo *) image_info->options));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%     I s C o m m a n d O p t i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsCommandOption() returns MagickTrue if the option begins with a - or + and
%  the first character that follows is alphanumeric.
%
%  The format of the IsCommandOption method is:
%
%      MagickBooleanType IsCommandOption(const char *option)
%
%  A description of each parameter follows:
%
%    o option: the option.
%
*/
MagickExport MagickBooleanType IsCommandOption(const char *option)
{
  assert(option != (const char *) NULL);
  if ((*option != '-') && (*option != '+'))
    return(MagickFalse);
  if (strlen(option) == 1)
    return(IsMagickTrue( (*option == '{') || (*option == '}')
                      || (*option == '[') || (*option == ']') ));
  option++;
  if (*option == '-')
    return(MagickTrue);
  if (isalpha((int) ((unsigned char) *option)) == 0)
    return(MagickFalse);
  return(MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m m a n d O p t i o n T o M n e m o n i c                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CommandOptionToMnemonic() returns an enumerated value as a mnemonic.
%
%  The format of the CommandOptionToMnemonic method is:
%
%      const char *CommandOptionToMnemonic(const CommandOption option,
%        const ssize_t type)
%
%  A description of each parameter follows:
%
%    o option: the option.
%
%    o type: one or more values separated by commas.
%
*/
MagickExport const char *CommandOptionToMnemonic(const CommandOption option,
  const ssize_t type)
{
  const OptionInfo
    *option_info;

  register ssize_t
    i;

  option_info=GetOptionInfo(option);
  if (option_info == (const OptionInfo *) NULL)
    return((const char *) NULL);
  for (i=0; option_info[i].mnemonic != (const char *) NULL; i++)
    if (type == option_info[i].type)
      break;
  if (option_info[i].mnemonic == (const char *) NULL)
    return("undefined");
  return(option_info[i].mnemonic);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t C o m m a n d O p t i o n s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListCommandOptions() lists the contents of enumerated option type(s).
%
%  The format of the ListCommandOptions method is:
%
%      MagickBooleanType ListCommandOptions(FILE *file,
%        const CommandOption option,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o file:  list options to this file handle.
%
%    o option:  list these options.
%
%    o exception:  return any errors or warnings in this structure.
%
*/
MagickExport MagickBooleanType ListCommandOptions(FILE *file,
  const CommandOption option,ExceptionInfo *magick_unused(exception))
{
  const OptionInfo
    *option_info;

  register ssize_t
    i;

  if (file == (FILE *) NULL)
    file=stdout;
  option_info=GetOptionInfo(option);
  if (option_info == (const OptionInfo *) NULL)
    return(MagickFalse);
  for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
  {
    if (option_info[i].stealth != MagickFalse)
      continue;
    (void) FormatLocaleFile(file,"%s\n",option_info[i].mnemonic);
  }
  return(MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e C h a n n e l O p t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseChannelOption() parses a string and returns an enumerated channel
%  type(s).
%
%  The format of the ParseChannelOption method is:
%
%      ssize_t ParseChannelOption(const char *channels)
%
%  A description of each parameter follows:
%
%    o options: One or more values separated by commas.
%
*/
MagickExport ssize_t ParseChannelOption(const char *channels)
{
  register ssize_t
    i;

  ssize_t
    channel;

  channel=ParseCommandOption(MagickChannelOptions,MagickTrue,channels);
  if (channel >= 0)
    return(channel);
  channel=0;
  for (i=0; i < (ssize_t) strlen(channels); i++)
  {
    switch (channels[i])
    {
      case 'A':
      case 'a':
      {
        channel|=AlphaChannel;
        break;
      }
      case 'B':
      case 'b':
      {
        channel|=BlueChannel;
        break;
      }
      case 'C':
      case 'c':
      {
        channel|=CyanChannel;
        break;
      }
      case 'g':
      case 'G':
      {
        channel|=GreenChannel;
        break;
      }
      case 'K':
      case 'k':
      {
        channel|=BlackChannel;
        break;
      }
      case 'M':
      case 'm':
      {
        channel|=MagentaChannel;
        break;
      }
      case 'o':
      case 'O':
      {
        channel|=AlphaChannel; /* depreciate */
        break;
      }
      case 'R':
      case 'r':
      {
        channel|=RedChannel;
        break;
      }
      case 'Y':
      case 'y':
      {
        channel|=YellowChannel;
        break;
      }
      case ',':
      {
        ssize_t
          type;

        /*
          Gather the additional channel flags and merge with shorthand.
        */
        type=ParseCommandOption(MagickChannelOptions,MagickTrue,channels+i+1);
        if (type < 0)
          return(type);
        channel|=type;
        return(channel);
      }
      default:
        return(-1);
    }
  }
  return(channel);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e C o m m a n d O p t i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseCommandOption() parses a string and returns an enumerated option
%  type(s).  Return a value of -1 if no such option is found.
%
%  The format of the ParseCommandOption method is:
%
%      ssize_t ParseCommandOption(const CommandOption option_table,
%        const MagickBooleanType list,const char *options)
%
%  A description of each parameter follows:
%
%    o option_table: Index to the option table to lookup
%
%    o list: A option other than zero permits more than one option separated by
%      a comma or pipe.
%
%    o options: One or more options separated by commas.
%
*/
MagickExport ssize_t ParseCommandOption(const CommandOption option_table,
  const MagickBooleanType list,const char *options)
{
  char
    token[MaxTextExtent];

  const OptionInfo
    *option_info;

  int
    sentinel;

  MagickBooleanType
    negate;

  register char
    *q;

  register const char
    *p;

  register ssize_t
    i;

  ssize_t
    option_types;

  option_info=GetOptionInfo(option_table);
  if (option_info == (const OptionInfo *) NULL)
    return(-1);
  option_types=0;
  sentinel=',';
  if (strchr(options,'|') != (char *) NULL)
    sentinel='|';
  for (p=options; p != (char *) NULL; p=strchr(p,sentinel))
  {
    while (((isspace((int) ((unsigned char) *p)) != 0) || (*p == sentinel)) &&
           (*p != '\0'))
      p++;
    negate=(*p == '!') ? MagickTrue : MagickFalse;
    if (negate != MagickFalse)
      p++;
    q=token;
    while (((isspace((int) ((unsigned char) *p)) == 0) && (*p != sentinel)) &&
           (*p != '\0'))
    {
      if ((q-token) >= (MaxTextExtent-1))
        break;
      *q++=(*p++);
    }
    *q='\0';
    for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
      if (LocaleCompare(token,option_info[i].mnemonic) == 0)
        {
          if (*token == '!')
            option_types=option_types &~ option_info[i].type;
          else
            option_types=option_types | option_info[i].type;
          break;
        }
    if ((option_info[i].mnemonic == (char *) NULL) &&
        ((strchr(token+1,'-') != (char *) NULL) ||
         (strchr(token+1,'_') != (char *) NULL)))
      {
        while ((q=strchr(token+1,'-')) != (char *) NULL)
          (void) CopyMagickString(q,q+1,MaxTextExtent-strlen(q));
        while ((q=strchr(token+1,'_')) != (char *) NULL)
          (void) CopyMagickString(q,q+1,MaxTextExtent-strlen(q));
        for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
          if (LocaleCompare(token,option_info[i].mnemonic) == 0)
            {
              if (*token == '!')
                option_types=option_types &~ option_info[i].type;
              else
                option_types=option_types | option_info[i].type;
              break;
            }
      }
    if (option_info[i].mnemonic == (char *) NULL)
      return(-1);
    if (list == MagickFalse)
      break;
  }
  return(option_types);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e P i x e l C h a n n e l O p t i o n                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParsePixelChannelOption() parses a string and returns an enumerated pixel
%  channel type(s).
%
%  The format of the ParsePixelChannelOption method is:
%
%      ssize_t ParsePixelChannelOption(const char *channels)
%
%  A description of each parameter follows:
%
%    o channels: One or more channels separated by commas.
%
*/
MagickExport ssize_t ParsePixelChannelOption(const char *channels)
{
  char
    *q,
    token[MaxTextExtent];

  ssize_t
    channel;

  GetMagickToken(channels,NULL,token);
  if ((*token == ';') || (*token == '|'))
    return(RedPixelChannel);
  channel=ParseCommandOption(MagickPixelChannelOptions,MagickTrue,token);
  if (channel >= 0)
    return(channel);
  q=(char *) token;
  channel=InterpretLocaleValue(token,&q);
  if ((q == token) || (channel < 0) || (channel >= MaxPixelChannels))
    return(-1);
  return(channel);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e m o v e I m a g e O p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveImageOption() removes an option from the image and returns its value.
%
%  In this case the ConstantString() value returned should be freed by the
%  caller when finished.
%
%  The format of the RemoveImageOption method is:
%
%      char *RemoveImageOption(ImageInfo *image_info,const char *option)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o option: the image option.
%
*/
MagickExport char *RemoveImageOption(ImageInfo *image_info,const char *option)
{
  char
    *value;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return((char *) NULL);
  value=(char *) RemoveNodeFromSplayTree((SplayTreeInfo *)
    image_info->options,option);
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s e t I m a g e O p t i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResetImageOptions() resets the image_info option.  That is, it deletes
%  all global options associated with the image_info structure.
%
%  The format of the ResetImageOptions method is:
%
%      ResetImageOptions(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
*/
MagickExport void ResetImageOptions(const ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return;
  ResetSplayTree((SplayTreeInfo *) image_info->options);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s e t I m a g e O p t i o n I t e r a t o r                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResetImageOptionIterator() resets the image_info values iterator.  Use it
%  in conjunction with GetNextImageOption() to iterate over all the values
%  associated with an image option.
%
%  The format of the ResetImageOptionIterator method is:
%
%      ResetImageOptionIterator(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
*/
MagickExport void ResetImageOptionIterator(const ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  if (image_info->options == (void *) NULL)
    return;
  ResetSplayTreeIterator((SplayTreeInfo *) image_info->options);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e O p t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageOption() associates an value with an image option.
%
%  The format of the SetImageOption method is:
%
%      MagickBooleanType SetImageOption(ImageInfo *image_info,
%        const char *option,const char *value)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o option: the image option.
%
%    o values: the image option values.
%
*/
MagickExport MagickBooleanType SetImageOption(ImageInfo *image_info,
  const char *option,const char *value)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  /*
    Specific global option settings.
  */
  if (LocaleCompare(option,"size") == 0)
    (void) CloneString(&image_info->size,value);
  /*
    Create tree if needed - specify how key,values are to be freed.
  */
  if (image_info->options == (void *) NULL)
    image_info->options=NewSplayTree(CompareSplayTreeString,
      RelinquishMagickMemory,RelinquishMagickMemory);
  /*
    Delete Option if NULL --  empty string values are valid!
  */
  if (value == (const char *) NULL)
    return(DeleteImageOption(image_info,option));
  /*
    Add option to splay-tree.
  */
  return(AddValueToSplayTree((SplayTreeInfo *) image_info->options,
    ConstantString(option),ConstantString(value)));
}
