/*
  Copyright 1999-2010 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.
  
  You may not use this file except in compliance with the License.
  obtain a copy of the License at
  
    http://www.imagemagick.org/script/license.php
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore static coder registration methods.
*/
#ifndef _MAGICKCORE_STATIC_H
#define _MAGICKCORE_STATIC_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern MagickExport MagickBooleanType
  InvokeStaticImageFilter(const char *,Image **,const int,const char **,
    ExceptionInfo *);

extern ModuleExport size_t
  RegisterARTImage(void),
  RegisterAVIImage(void),
  RegisterAVSImage(void),
  RegisterBIEImage(void),
  RegisterBMPImage(void),
  RegisterBRAILLEImage(void),
  RegisterCALSImage(void),
  RegisterCAPTIONImage(void),
  RegisterCINImage(void),
  RegisterCIPImage(void),
  RegisterCLIPImage(void),
  RegisterCLIPBOARDImage(void),
  RegisterCMYKImage(void),
  RegisterCUTImage(void),
  RegisterDCMImage(void),
  RegisterDCXImage(void),
  RegisterDDSImage(void),
  RegisterDIBImage(void),
  RegisterDJVUImage(void),
  RegisterDNGImage(void),
  RegisterDPSImage(void),
  RegisterDPXImage(void),
  RegisterEMFImage(void),
  RegisterEPDFImage(void),
  RegisterEPIImage(void),
  RegisterEPSImage(void),
  RegisterEPS2Image(void),
  RegisterEPSFImage(void),
  RegisterEPSIImage(void),
  RegisterEPTImage(void),
  RegisterEXRImage(void),
  RegisterFAXImage(void),
  RegisterFITSImage(void),
  RegisterFPXImage(void),
  RegisterG3Image(void),
  RegisterGIFImage(void),
  RegisterGIF87Image(void),
  RegisterGRADIENTImage(void),
  RegisterGRANITEImage(void),
  RegisterGRAYImage(void),
  RegisterHImage(void),
  RegisterHISTOGRAMImage(void),
  RegisterHRZImage(void),
  RegisterHTMLImage(void),
  RegisterICBImage(void),
  RegisterICONImage(void),
  RegisterINFOImage(void),
  RegisterINLINEImage(void),
  RegisterIPLImage(void),
  RegisterJBGImage(void),
  RegisterJBIGImage(void),
  RegisterJPGImage(void),
  RegisterJPEGImage(void),
  RegisterJP2Image(void),
  RegisterLABELImage(void),
  RegisterMAGICKImage(void),
  RegisterMAPImage(void),
  RegisterMATImage(void),
  RegisterMATTEImage(void),
  RegisterMETAImage(void),
  RegisterMIFFImage(void),
  RegisterMNGImage(void),
  RegisterMONOImage(void),
  RegisterMPCImage(void),
  RegisterMPEGImage(void),
  RegisterMPRImage(void),
  RegisterMSLImage(void),
  RegisterMTVImage(void),
  RegisterMVGImage(void),
  RegisterNETSCAPEImage(void),
  RegisterNULLImage(void),
  RegisterP7Image(void),
  RegisterPBMImage(void),
  RegisterOTBImage(void),
  RegisterPALMImage(void),
  RegisterPATTERNImage(void),
  RegisterPCDImage(void),
  RegisterPCDSImage(void),
  RegisterPCLImage(void),
  RegisterPCTImage(void),
  RegisterPCXImage(void),
  RegisterPDBImage(void),
  RegisterPDFImage(void),
  RegisterPESImage(void),
  RegisterPICImage(void),
  RegisterPICTImage(void),
  RegisterPIXImage(void),
  RegisterPGMImage(void),
  RegisterPLASMAImage(void),
  RegisterPMImage(void),
  RegisterPNGImage(void),
  RegisterPNMImage(void),
  RegisterPPMImage(void),
  RegisterPREVIEWImage(void),
  RegisterPSImage(void),
  RegisterPS2Image(void),
  RegisterPS3Image(void),
  RegisterPSDImage(void),
  RegisterPTIFImage(void),
  RegisterPWPImage(void),
  RegisterRASImage(void),
  RegisterRAWImage(void),
  RegisterRGBImage(void),
  RegisterRGBAImage(void),
  RegisterRLAImage(void),
  RegisterRLEImage(void),
  RegisterSCRImage(void),
  RegisterSCTImage(void),
  RegisterSFWImage(void),
  RegisterSGIImage(void),
  RegisterSHTMLImage(void),
  RegisterSTEGANOImage(void),
  RegisterSUNImage(void),
  RegisterSVGImage(void),
  RegisterTEXTImage(void),
  RegisterTGAImage(void),
  RegisterTHUMBNAILImage(void),
  RegisterTIFImage(void),
  RegisterTIFFImage(void),
  RegisterTILEImage(void),
  RegisterTIMImage(void),
  RegisterTTFImage(void),
  RegisterTXTImage(void),
  RegisterUILImage(void),
  RegisterURLImage(void),
  RegisterUYVYImage(void),
  RegisterVDAImage(void),
  RegisterVICARImage(void),
  RegisterVIDImage(void),
  RegisterVIFFImage(void),
  RegisterVSTImage(void),
  RegisterWBMPImage(void),
  RegisterWMFImage(void),
  RegisterWPGImage(void),
  RegisterXImage(void),
  RegisterXBMImage(void),
  RegisterXCImage(void),
  RegisterXCFImage(void),
  RegisterXPMImage(void),
  RegisterXPSImage(void),
  RegisterXTRNImage(void),
  RegisterXVImage(void),
  RegisterXWDImage(void),
  RegisterYCBCRImage(void),
  RegisterYUVImage(void);

extern ModuleExport void
  UnregisterARTImage(void),
  UnregisterAVIImage(void),
  UnregisterAVSImage(void),
  UnregisterBIEImage(void),
  UnregisterBMPImage(void),
  UnregisterBRAILLEImage(void),
  UnregisterCALSImage(void),
  UnregisterCAPTIONImage(void),
  UnregisterCINImage(void),
  UnregisterCIPImage(void),
  UnregisterCLIPImage(void),
  UnregisterCLIPBOARDImage(void),
  UnregisterCMYKImage(void),
  UnregisterCUTImage(void),
  UnregisterDCMImage(void),
  UnregisterDCXImage(void),
  UnregisterDDSImage(void),
  UnregisterDIBImage(void),
  UnregisterDJVUImage(void),
  UnregisterDNGImage(void),
  UnregisterDPSImage(void),
  UnregisterDPXImage(void),
  UnregisterEMFImage(void),
  UnregisterEPDFImage(void),
  UnregisterEPIImage(void),
  UnregisterEPSImage(void),
  UnregisterEPS2Image(void),
  UnregisterEPSFImage(void),
  UnregisterEPSIImage(void),
  UnregisterEPTImage(void),
  UnregisterEXRImage(void),
  UnregisterFAXImage(void),
  UnregisterFITSImage(void),
  UnregisterFPXImage(void),
  UnregisterG3Image(void),
  UnregisterGIFImage(void),
  UnregisterGIF87Image(void),
  UnregisterGRADIENTImage(void),
  UnregisterGRANITEImage(void),
  UnregisterGRAYImage(void),
  UnregisterHImage(void),
  UnregisterHISTOGRAMImage(void),
  UnregisterHRZImage(void),
  UnregisterHTMLImage(void),
  UnregisterICBImage(void),
  UnregisterICONImage(void),
  UnregisterINFOImage(void),
  UnregisterINLINEImage(void),
  UnregisterIPLImage(void),
  UnregisterJBGImage(void),
  UnregisterJBIGImage(void),
  UnregisterJPGImage(void),
  UnregisterJPEGImage(void),
  UnregisterJP2Image(void),
  UnregisterLABELImage(void),
  UnregisterLOCALEImage(void),
  UnregisterMAGICKImage(void),
  UnregisterMAPImage(void),
  UnregisterMATImage(void),
  UnregisterMATTEImage(void),
  UnregisterMETAImage(void),
  UnregisterMIFFImage(void),
  UnregisterMNGImage(void),
  UnregisterMONOImage(void),
  UnregisterMPCImage(void),
  UnregisterMPEGImage(void),
  UnregisterMPRImage(void),
  UnregisterMSLImage(void),
  UnregisterMTVImage(void),
  UnregisterMVGImage(void),
  UnregisterNETSCAPEImage(void),
  UnregisterNULLImage(void),
  UnregisterP7Image(void),
  UnregisterPBMImage(void),
  UnregisterOTBImage(void),
  UnregisterPALMImage(void),
  UnregisterPATTERNImage(void),
  UnregisterPCDImage(void),
  UnregisterPCDSImage(void),
  UnregisterPCLImage(void),
  UnregisterPCTImage(void),
  UnregisterPCXImage(void),
  UnregisterPDBImage(void),
  UnregisterPDFImage(void),
  UnregisterPESImage(void),
  UnregisterPICImage(void),
  UnregisterPICTImage(void),
  UnregisterPIXImage(void),
  UnregisterPLASMAImage(void),
  UnregisterPGMImage(void),
  UnregisterPMImage(void),
  UnregisterPNGImage(void),
  UnregisterPNMImage(void),
  UnregisterPPMImage(void),
  UnregisterPREVIEWImage(void),
  UnregisterPSImage(void),
  UnregisterPS2Image(void),
  UnregisterPS3Image(void),
  UnregisterPSDImage(void),
  UnregisterPTIFImage(void),
  UnregisterPWPImage(void),
  UnregisterRASImage(void),
  UnregisterRAWImage(void),
  UnregisterRGBImage(void),
  UnregisterRGBAImage(void),
  UnregisterRLAImage(void),
  UnregisterRLEImage(void),
  UnregisterSCRImage(void),
  UnregisterSCTImage(void),
  UnregisterSFWImage(void),
  UnregisterSGIImage(void),
  UnregisterSHTMLImage(void),
  UnregisterSTEGANOImage(void),
  UnregisterSUNImage(void),
  UnregisterSVGImage(void),
  UnregisterTEXTImage(void),
  UnregisterTGAImage(void),
  UnregisterTHUMBNAILImage(void),
  UnregisterTIFImage(void),
  UnregisterTIFFImage(void),
  UnregisterTILEImage(void),
  UnregisterTIMImage(void),
  UnregisterTTFImage(void),
  UnregisterTXTImage(void),
  UnregisterUILImage(void),
  UnregisterURLImage(void),
  UnregisterUYVYImage(void),
  UnregisterVDAImage(void),
  UnregisterVICARImage(void),
  UnregisterVIDImage(void),
  UnregisterVIFFImage(void),
  UnregisterVSTImage(void),
  UnregisterWBMPImage(void),
  UnregisterWMFImage(void),
  UnregisterWPGImage(void),
  UnregisterXImage(void),
  UnregisterXBMImage(void),
  UnregisterXCImage(void),
  UnregisterXCFImage(void),
  UnregisterXPMImage(void),
  UnregisterXPSImage(void),
  UnregisterXTRNImage(void),
  UnregisterXVImage(void),
  UnregisterXWDImage(void),
  UnregisterYCBCRImage(void),
  UnregisterYUVImage(void);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
