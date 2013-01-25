import os,struct,binascii,zlib

class Error(Exception):
    pass

allowed_extensions = set(('.kf','.nif','.dds','.wav','.tai','.fnt','.tex','.psd','.db','.xml','.txt'))

def tesHash(fileName,folder = False):
    """Returns tes4's two hash values for filename.
    Based on TimeSlips code with cleanup and pythonization."""
    root,ext = os.path.splitext(fileName.lower()) #--"bob.dds" >> root = "bob", ext = ".dds"
    if folder:
        root = root + ext
        ext = ''
        #--Hash1
    chars = map(ord,root) #--'bob' >> chars = [98,111,98]
    try:
        hash1 = chars[-1] | (0,chars[-2])[len(chars)>2]<<8 | len(chars)<<16 | chars[0]<<24
    except IndexError:
        hash1 = chars[-1] | len(chars)<<16 | chars[0]<<24
    #--(a,b)[test] is similar to test?a:b in C. (Except that evaluation is not shortcut.)
    if   ext == '.kf':  hash1 |= 0x80
    elif ext == '.nif': hash1 |= 0x8000
    elif ext == '.dds': hash1 |= 0x8080
    elif ext == '.wav': hash1 |= 0x80000000
    #--Hash2
    #--Python integers have no upper limit. Use uintMask to restrict these to 32 bits.
    uintMask, hash2, hash3 = 0xFFFFFFFF, 0, 0 
    for char in chars[1:-2]: #--Slice of the chars array
        hash2 = ((hash2 * 0x1003f) + char ) & uintMask
    for char in map(ord,ext):
        hash3 = ((hash3 * 0x1003F) + char ) & uintMask
    hash2 = (hash2 + hash3) & uintMask
    #--Done
    return (hash2<<32) + hash1 #--Return as uint64

class FolderRecord(object):
    def __init__(self,f):
        self.name_hash = f.read(8)
        self.count,self.offset = struct.unpack('II',f.read(8))

class FileRecord(object):
    def __init__(self,f,folder_name,default_compressed):
        self.f = f
        self.name_hash   = f.read(8)
        self.folder_name = folder_name
        self.size,self.offset = struct.unpack('II',f.read(8))
        self.compressed = ((self.size>>30)&1) ^ default_compressed
        self.size &= (~(1<<30))

    def read(self):
        #print self.name,hex(self.offset),self.compressed
        self.f.seek(self.offset)
        if self.compressed:
            name_size = ord(self.f.read(1))
            name = self.f.read(name_size)
            decompress_size = struct.unpack('I',self.f.read(4))[0]
            data = self.f.read(self.size)
            decomp = zlib.decompress(data)
            assert len(decomp) == decompress_size
            return decomp
        else:
            raise NotDoneYet

class BSAFile(object):
    def __init__(self,filename):
        self.f = open(filename,'rb')
        f = self.f
        if f.read(4) != 'BSA\x00':
            raise Error('Not a BSA File')
        self.version                  , \
        self.offset                   , \
        self.archive_flags            , \
        self.folder_count             , \
        self.file_count               , \
        self.total_folder_name_length , \
        self.total_file_name_length   , \
        self.file_flags = struct.unpack('I'*8,f.read(4*8))
        self.folder_records = {}
        self.file_records = {}
        self.default_compressed = (self.archive_flags>>2)&1
        for i in xrange(self.folder_count):
            new_record = FolderRecord(f)
            self.folder_records[new_record.name_hash] = new_record
        for i in xrange(self.folder_count):
            folder_name = []
            c = f.read(1) #that's the length
            c = f.read(1)
            while c and c != '\x00':
                folder_name.append(c)
                c = f.read(1)
            folder_name = ''.join(folder_name)
            h = tesHash(folder_name,folder=True)
            hs = struct.pack('Q',h)
            #print folder_name
            folder_record = self.folder_records[hs]
            folder_record.name = folder_name
            folder_record.files = {}
            for i in xrange(folder_record.count):
                new_record = FileRecord(f,folder_name,self.default_compressed)
                folder_record.files[new_record.name_hash] = new_record
                self.file_records[new_record.name_hash] = new_record

        count = 0
        while count < self.file_count:
            file_name = []
            c = f.read(1)
            while c and c != '\x00':
                file_name.append(c)
                c = f.read(1)
            file_name = ''.join(file_name)
            #print hex(f.tell()),file_name
            h = tesHash(file_name)
            hs = struct.pack('Q',h)
            self.file_records[hs].name = file_name
            count += 1

if __name__ == '__main__':
    import sys
    import cStringIO as StringIO
    import Image
    import DDSImageFile
    import dds
    import zipfile
    import re

    essential = set(('special_luck.dds'          ,
                     'special_intelligence.dds'  ,
                     'special_endurance.dds'     ,
                     'special_strength.dds'      ,
                     'special_perception.dds'    ,
                     'special_charisma.dds'      ,
                     'special_agility.dds'       ,
                     'skills_barter.dds'         ,
                     'skills_energy_weapons.dds' ,
                     'skills_small_guns.dds'     ,
                     'skills_lockpick.dds'       ,
                     'skills_medicine.dds'       ,
                     'skills_melee_weapons.dds'  ,
                     'skills_repair.dds'         ,
                     'skills_science.dds'        ,
                     'skills_sneak.dds'          ,
                     'skills_speech.dds'         ,
                     'skills_survival.dds'       ,
                     'skills_unarmed.dds'        ,
                     'monofonto_verylarge02_dialogs2_0_lod_a.dds'))

    other_wanted = set(('power_armor.dds' ,
                        'right_leg.dds'   ,
                        'right_arm.dds'   ,
                        'torso.dds'       ,
                        'head.dds'        ,
                        'left_leg.dds'    ,
                        'left_arm.dds'    ,
                        'forward.dds'     ,
                        'backward.dds'    ,
                        'left.dds'        ,
                        'right.dds'       ,
                        'monofonto_verylarge02_dialogs2_0_lod_a.dds'))

    #start the output zipfile
    with zipfile.ZipFile(sys.argv[2],'w') as output_zip:
        bsa = BSAFile(sys.argv[1])
        out = sys.argv[2]
        #print bsa.version
        for file_record in bsa.file_records.values():
            print file_record.name
            if re.match('(perk|special|reputations|skills|weap|weapons|vault_suit|item|items|apperal|appearal|apparel|missile)_.*\.dds',file_record.name) or file_record.name in other_wanted:
                data = file_record.read()
                data_file = StringIO.StringIO(data)
                im = Image.open(data_file)
                png_data = StringIO.StringIO()
                im.save(png_data,format = 'PNG')
                png_data = png_data.getvalue()
                new_name = file_record.name.replace('.dds','.png')
                output_zip.writestr(new_name,png_data)
                print file_record.name,len(png_data)
                try:
                    essential.remove(file_record.name)
                except KeyError:
                    pass
                #print im
                #with open(out,'wb') as f:
                #    f.write(data)
                #raise SystemExit

    for item in essential:
        print 'missing',item
