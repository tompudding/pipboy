# DdsImageFile.py -- a PIL loader for (some) .DDS (DirectX8 texture) files
# See http://www.randomly.org/projects/misc/ for updates.

# Copyright (c) 2002, Oliver Jowett <oliver@randomly.org>
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# This is a PIL loader for the .dds DirectX file format
#
# The DDS file format is described (somewhat) at:
#  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dx8_vb/directx_vb/Graphics/ProgrammersGuide/Appendix/DDSFileFormat/ovwDDSFileFormat.asp
#
# The DTX1 texture format is described at:
#  http://oss.sgi.com/projects/ogl-sample/registry/EXT/texture_compression_s3tc.txt

import Image, ImageFile, struct, string

dxt1Decoder = None
    
# dwFlags constants
DDSD_CAPS = 1
DDSD_HEIGHT = 2
DDSD_WIDTH = 4
DDSD_PITCH = 8
DDSD_PIXELFORMAT = 0x1000
DDSD_MIPMAPCOUNT = 0x20000
DDSD_LINEARSIZE = 0x80000
DDSD_DEPTH = 0x800000

DDSD_EXPECTED = DDSD_CAPS+DDSD_HEIGHT+DDSD_WIDTH+DDSD_PIXELFORMAT

# ddpfPixelFormat.dwFlags constants
DDPF_ALPHAPIXELS = 1
DDPF_FOURCC = 4
DDPF_RGB = 0x40
    
# ddsCaps.dwCaps1 constants
DDSCAPS_COMPLEX = 8
DDSCAPS_TEXTURE = 0x1000
DDSCAPS_MIPMAP = 0x00400000
    
DDSCAPS_EXPECTED = DDSCAPS_TEXTURE

# Python-only DXT1 decoder; this is slow!
# Better to use _dxt1.decodeDXT1 if you can
# (it's used automatically if available by DdsImageFile)
def pythonDecodeDXT1(data):
    # input: one "row" of data (i.e. will produce 4*width pixels)
    blocks = len(data) / 8  # number of blocks in row
    out = ['', '', '', '']  # row accumulators

    for xb in xrange(blocks):
        # Decode next 8-byte block.        
        c0, c1, bits = struct.unpack('<HHI', data[xb*8:xb*8+8])

        # color 0, packed 5-6-5
        b0 = (c0 & 0x1f) << 3
        g0 = ((c0 >> 5) & 0x3f) << 2
        r0 = ((c0 >> 11) & 0x1f) << 3
        
        # color 1, packed 5-6-5
        b1 = (c1 & 0x1f) << 3
        g1 = ((c1 >> 5) & 0x3f) << 2
        r1 = ((c1 >> 11) & 0x1f) << 3

        # Decode this block into 4x4 pixels
        # Accumulate the results onto our 4 row accumulators
        for yo in xrange(4):
            for xo in xrange(4):
                # get next control op and generate a pixel
                
                control = bits & 3
                bits = bits >> 2
                if control == 0:
                    out[yo] += chr(r0) + chr(g0) + chr(b0)
                elif control == 1:
                    out[yo] += chr(r1) + chr(g1) + chr(b1)
                elif control == 2:                                
                    if c0 > c1:
                        out[yo] += chr((2 * r0 + r1 + 1) / 3) + chr((2 * g0 + g1 + 1) / 3) + chr((2 * b0 + b1 + 1) / 3)
                    else:
                        out[yo] += chr((r0 + r1) / 2) + chr((g0 + g1) / 2) + chr((b0 + b1) / 2)
                elif control == 3:
                    if c0 > c1:
                        out[yo] += chr((2 * r1 + r0 + 1) / 3) + chr((2 * g1 + g0 + 1) / 3) + chr((2 * b1 + b0 + 1) / 3)
                    else:
                        out[yo] += '\0\0\0'

    # All done.
    return tuple(out)

def pythonDecodeDXT3(data):
    # input: one "row" of data (i.e. will produce 4*width pixels)
    blocks = len(data) / 16  # number of blocks in row
    out = ['', '', '', '']  # row accumulators

    for xb in xrange(blocks):
        # Decode next 8-byte block.        
        alpha, c0, c1, bits = struct.unpack('<QHHI', data[xb*16:(xb+1)*16])

        # color 0, packed 5-6-5
        b0 = (c0 & 0x1f) << 3
        g0 = ((c0 >> 5) & 0x3f) << 2
        r0 = ((c0 >> 11) & 0x1f) << 3
        
        # color 1, packed 5-6-5
        b1 = (c1 & 0x1f) << 3
        g1 = ((c1 >> 5) & 0x3f) << 2
        r1 = ((c1 >> 11) & 0x1f) << 3

        r0,b0,g0,r1,b1,g1 = ((int(v*1.2) if v*1.2 < 256 else 0xff) for v in (r0,b0,g0,r1,b1,g1))

        # Decode this block into 4x4 pixels
        # Accumulate the results onto our 4 row accumulators
        print hex(alpha)
        for yo in xrange(4):
            for xo in xrange(4):
                # get next control op and generate a pixel
                
                control = bits & 3
                bits = bits >> 2
                pixel_alpha = (alpha&0xf)*20
                if (pixel_alpha > 192):
                    print pixel_alpha
                alpha >>= 4
                if control == 0:
                    out[yo] += chr(r0) + chr(g0) + chr(b0) + chr(pixel_alpha)
                elif control == 1:
                    out[yo] += chr(r1) + chr(g1) + chr(b1) + chr(pixel_alpha)
                elif control == 2:                                
                    if 1:#c0 > c1:
                        out[yo] += chr((2 * r0 + r1 + 1) / 3) + chr((2 * g0 + g1 + 1) / 3) + chr((2 * b0 + b1 + 1) / 3) + chr(pixel_alpha)
                    else:
                        out[yo] += chr((r0 + r1) / 2) + chr((g0 + g1) / 2) + chr((b0 + b1) / 2) + chr(pixel_alpha)
                elif control == 3:
                    if 1:#c0 > c1:
                        out[yo] += chr((2 * r1 + r0 + 1) / 3) + chr((2 * g1 + g0 + 1) / 3) + chr((2 * b1 + b0 + 1) / 3) + chr(pixel_alpha)
                    else:
                        out[yo] += '\0\0\0\0'

    # All done.
    return tuple(out)

def pythonDecodeDXT5(data):
    # input: one "row" of data (i.e. will produce 4*width pixels)
    blocks = len(data) / 16  # number of blocks in row
    out = ['', '', '', '']  # row accumulators
    a = [0 for i in xrange(8)]

    for xb in xrange(blocks):
        # Decode next 8-byte block.        
        a[0],a[1],tab1,tab2, c0, c1, bits = struct.unpack('<BBHIHHI', data[xb*16:(xb+1)*16])

        atab = (tab2<<16) | tab1

        if a[0] > a[1]:
            for i in xrange(2,8):
                a[i] = ((8-i)*a[0] + (i-1)*a[1])/7
        else:
            for i in xrange(2,6):
                a[i] = ((6-i)*a[0] + (i-1)*a[1])/5
            a[6] = 0
            a[7] = 0xff

        a = [chr(v) for v in a]

        # color 0, packed 5-6-5
        b0 = (c0 & 0x1f) << 3
        g0 = ((c0 >> 5) & 0x3f) << 2
        r0 = ((c0 >> 11) & 0x1f) << 3
        
        # color 1, packed 5-6-5
        b1 = (c1 & 0x1f) << 3
        g1 = ((c1 >> 5) & 0x3f) << 2
        r1 = ((c1 >> 11) & 0x1f) << 3

        r0,b0,g0,r1,b1,g1 = ((int(v*1.2) if v*1.2 < 256 else 0xff) for v in (r0,b0,g0,r1,b1,g1))

        # Decode this block into 4x4 pixels
        # Accumulate the results onto our 4 row accumulators
        for yo in xrange(4):
            for xo in xrange(4):
                # get next control op and generate a pixel
                
                control = bits & 3
                bits = bits >> 2
                alpha_control = atab&7
                atab >>= 3

                if control == 0:
                    out[yo] += chr(r0) + chr(g0) + chr(b0) + a[alpha_control]
                elif control == 1:
                    out[yo] += chr(r1) + chr(g1) + chr(b1) + a[alpha_control]
                elif control == 2:                                
                    if 1:#c0 > c1:
                        out[yo] += chr((2 * r0 + r1 + 1) / 3) + chr((2 * g0 + g1 + 1) / 3) + chr((2 * b0 + b1 + 1) / 3) + a[alpha_control]
                    else:
                        out[yo] += chr((r0 + r1) / 2) + chr((g0 + g1) / 2) + chr((b0 + b1) / 2) + a[alpha_control]
                elif control == 3:
                    if 1:#c0 > c1:
                        out[yo] += chr((2 * r1 + r0 + 1) / 3) + chr((2 * g1 + g0 + 1) / 3) + chr((2 * b1 + b0 + 1) / 3) + a[alpha_control]
                    else:
                        out[yo] += '\0\0\0\0'

    # All done.
    return tuple(out)


class DdsImageFile(ImageFile.ImageFile):
    format = 'DDS'
    format_description = 'DirectX8 DDS texture file'

    def _open(self):
        self._loaded = 0
        
        # Check header
        header = self.fp.read(128)
        if header[:4] != 'DDS ':
            raise SyntaxError, 'Not a DDS file'

        dwSize, dwFlags, dwHeight, dwWidth, dwPitchLinear, dwDepth, dwMipMapCount, ddpfPixelFormat, ddsCaps = struct.unpack('<IIIIIII 44x 32s 16s 4x', header[4:])
        self.size = dwWidth, dwHeight

        if dwSize != 124:
            raise SyntaxError, 'Expected dwSize == 124, got %d' % dwSize

        if (dwFlags & DDSD_EXPECTED) != DDSD_EXPECTED:
            raise SyntaxError, 'Unsupported image flags: %08x' % dwFlags
        
        pf_dwSize, pf_dwFlags, pf_dwFourCC, pf_dwRGBBitCount, pf_dwRBitMask, pf_dwGBitMask, pf_dwBBitMask, pf_dwRGBAlphaBitMask = struct.unpack('<II4sIIIII', ddpfPixelFormat)
        if pf_dwSize != 32:
            raise SyntaxError, 'Expected pf_dwSize == 32, got %d' % pf_dwSize

        caps_dwCaps1, caps_dwCaps2 = struct.unpack('<II 8x', ddsCaps)
        if (caps_dwCaps1 & DDSCAPS_EXPECTED) != DDSCAPS_EXPECTED:
            raise SyntaxError, 'Unsupported image caps: %08x' % caps_dwCaps1

        # check for DXT1
        if (pf_dwFlags & DDPF_FOURCC != 0):
            if pf_dwFourCC == 'DXT1':
                if (pf_dwFlags & DDPF_ALPHAPIXELS != 0):
                    raise SyntaxError, 'DXT1 with Alpha not supported' # yet
                else:
                    self.mode = 'RGB'
                    self.dxt_decoder = pythonDecodeDXT1
                    self.blocksize = 8
                    self.modesize = 3
                    self.load = self._loadDXTOpaque
            elif pf_dwFourCC == 'DXT3':
                self.mode = 'RGBA'
                self.dxt_decoder = pythonDecodeDXT3
                self.blocksize = 16
                self.modesize = 4
                self.load = self._loadDXTOpaque
            elif pf_dwFourCC == 'DXT5':
                self.mode = 'RGBA'
                self.dxt_decoder = pythonDecodeDXT5
                self.blocksize = 16
                self.modesize = 4
                self.load = self._loadDXTOpaque
            else:
                print 'Unsupported FOURCC mode: %s' % pf_dwFourCC
                raise SyntaxError, 'Unsupported FOURCC mode: %s' % pf_dwFourCC
        else:
            # XXX is this right? I don't have an uncompressed dds to play with
            self.mode = 'RGB'
            # Construct the tile.
            self.tile = [('raw', (0,0,dwWidth,dwHeight), 128,
                          ('RGBX', dwPitchLinear - dwWidth, 1))]

    def _loadDXTOpaque(self):
        #global dxt1Decoder
        
        if self._loaded: return

        #if not dxt1Decoder:
        	# dxt1Decoder = pythonDecodeDXT1
            # Haven't selected a decoder yet. Try to get the C implementation
        #    try:
        #        import _dxt1
        #        dxt1Decoder = _dxt1.decodeDXT1
        #    except ImportError:
        #        print "WARNING: Failed to import _dxt1 module."
        #        print "WARNING: I'll use the slow python-based decoder instead."
        #        dxt1Decoder = pythonDecodeDXT1
        

        # one entry per Y row, we join them up at the end
        data = []
        self.fp.seek(128) # skip header

        linesize = (self.size[0] + 3) / 4 * self.blocksize # Number of data byte per row

        baseoffset = 0
        for yb in xrange((self.size[1] + 3) / 4):             
            linedata = self.fp.read(linesize)
            decoded = self.dxt_decoder(linedata) # returns 4-tuple of RGB lines
            for d in decoded:
                # Make sure that if we have a texture size that's not a
                # multiple of 4 that we correctly truncate the returned data
                data.append(d[:self.size[0]*self.modesize])

        # Now build the final image from our data strings
        data = string.join(data[:self.size[1]],'')
        self.im = Image.core.new(self.mode, self.size)
        self.fromstring(data)
        self._loaded = 1

Image.register_open('DDS', DdsImageFile)
Image.register_extension('DDS', '.dds')
