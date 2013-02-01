import os
import struct
import binascii
import zlib
import tempfile
import glob

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
            return self.f.read(self.size)

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

#bit of a waste declaring this every time
essential_dds = set(('special_luck.dds'                           ,
                     'special_intelligence.dds'                   ,
                     'special_endurance.dds'                      ,
                     'special_strength.dds'                       ,
                     'special_perception.dds'                     ,
                     'special_charisma.dds'                       ,
                     'special_agility.dds'                        ,
                     'skills_barter.dds'                          ,
                     'skills_energy_weapons.dds'                  ,
                     'skills_small_guns.dds'                      ,
                     'skills_lockpick.dds'                        ,
                     'skills_medicine.dds'                        ,
                     'skills_melee_weapons.dds'                   ,
                     'skills_repair.dds'                          ,
                     'skills_science.dds'                         ,
                     'skills_sneak.dds'                           ,
                     'skills_speech.dds'                          ,
                     'skills_survival.dds'                        ,
                     'skills_unarmed.dds'                         ,
                     'screenglare.dds'                            ,
                     'monofonto_verylarge02_dialogs2_0_lod_a.dds' ,
                     'power_armor.dds'                            ,
                     'right_leg.dds'                              ,
                     'right_arm.dds'                              ,
                     'torso.dds'                                  ,
                     'head.dds'                                   ,
                     'wave.dds'                                   ,
                     'left_leg.dds'                               ,
                     'left_arm.dds'                               ,
                     'forward.dds'                                ,
                     'backward.dds'                               ,
                     #'left.dds'                                   ,
                     'right.dds'                                  ,
                     'weap_skill_icon_explosives.dds'             ,
                     'weap_skill_icon_energy.dds'                 ,
                     'weap_skill_icon_big_guns.dds'               ,
                     'weap_skill_icon_unarmed.dds'                ,
                     'weap_skill_icon_sm_arms.dds'                ,
                     'reputations_novac.dds'                      ,
                     'item_antivenom.dds'                         ,
                     'lincolnrifleammo.dds'                       ,
                     'face_00.dds'                                ,
                     'weap_skill_icon_melee.dds'                  ))


def screenglare_filter(im):
    #the screenglare needs converting so black becomes completely transparent,
    #and white is opaque
    inpix = im.getdata()
    out = Image.new('RGBA',im.size)
    outpix = out.load()
    print im,outpix
    w,h = im.size
    for x in xrange(w):
        for y in xrange(h):
            r,g,b = inpix[y*im.size[0] + x]
            z = float(max((r,g,b)))
            if z == 0:
                outpix[(x,y)] = (0,0,0,0)
            else:
                outpix[(x,y)] = (int((r/z)*255),int((g/z)*255),int((b/z)*255),int(z))
    return out

zip_file_cache = set()

def writeStrToZip(zip_file,filename,data):
    if filename in zip_file_cache:
        return
    zip_file.writestr(filename,data)
    zip_file_cache.add(filename)

def writeToZip(zip_file,local_filename,zip_filename):
    if zip_filename in zip_file_cache:
        return
    zip_file.write(local_filename,zip_filename)
    zip_file_cache.add(zip_filename)

def HandleDDS(file_name,extension,file_record,output_zip):

    zip_path = ''
    if file_record.name in essential_dds:
        zip_path = ''
        match = True
    else:
        match = re.match('(perk|special|reputations|skills|weap_skill|weapons|vault_suit|item|items|apperal|appearal|apparel|missile)_.*\.dds',file_record.name)
        if not match:
            return
        if match.groups()[0] in items_prefix:
            zip_path = 'icons'
        else:
            zip_path = ''
    if match:
        data = file_record.read()
        #print 'x',len(data)
        data_file = StringIO.StringIO(data)
        im = Image.open(data_file)
        if 'screenglare' in file_record.name:
            im = screenglare_filter(im)
            file_name = 'screenglare_alpha'
        if 'monofonto_verylarge' in file_record.name:
            file_name = 'monofonto_verylarge02_dialogs2'
        if 'lincolnrifle' in file_record.name:
            zip_path = 'icons'
            file_name = 'items__308_ammo'
        if 'face_00' in file_record.name:
            file_name = 'face'
        if 'antivenom' in file_record.name:
            zip_path = 'icons'
        png_data = StringIO.StringIO()
        im.save(png_data,format = 'PNG')
        png_data = png_data.getvalue()
        new_name = os.path.join(zip_path,file_name + '.png')
        writeStrToZip(output_zip,new_name,png_data)
        print file_record.name,len(png_data)
        try:
            essential_dds.remove(file_record.name)
        except KeyError:
            pass

music_files = []
    
def HandleMusic(file_name,extension,file_record,output_zip):
    if not file_name.startswith('mus_'):
        return
    print 'music file!',file_name,extension
    inputf = tempfile.NamedTemporaryFile(mode='wb', delete=False)
    inputf.write(file_record.read())
    inputf.flush()
    inputf.close()
    try:
        outputf = tempfile.NamedTemporaryFile(mode='wb', delete=False)
        outputf.close()
        try:
            song = AudioSegment.from_file(inputf.name,extension[1:])
            song = song.set_channels(1)
            song = song.set_frame_rate(22050)
            song.export(outputf.name,'s16le',codec = 'pcm_s16le')
            zip_path = os.path.join('music',file_name + '.snd')
            writeToZip(output_zip,outputf.name,zip_path)
            music_files.append(zip_path)
        finally:
            os.unlink(outputf.name)
    finally:
        os.unlink(inputf.name)

wavmap = {'ui_pipboy_tab'          : ['menu_tab'],
          'ui_pipboy_mode'         : ['mode_change'],
          'ui_menu_prevnext'       : ['menu_prevnext'],
          'wpn_pistol10mm_equip'   : ['equip_weapon'],
          'ui_pipboy_select'       : ['select_sound'],
          'wpn_pistol10mm_unequip' : ['unequip_weapon'],
          'ui_pipboy_access_up'    : ['equip_misc','equip_apparel','unequip_apparel'],
          'ui_pipboy_access_down'  : ['equip_aid']}

essential_sounds = set()
for name_list in wavmap.values():
    for name in name_list:
        essential_sounds.add(name + '.snd')

for tag in 'cd':
    for num in xrange(1,6):
        essential_sounds.add('ui_static_%s_%02d.wav.snd' % (tag,num))
          
def HandleWav(file_name,extension,file_record,output_zip):
    try:
        output_file_names = wavmap[file_name]
    except:
        if 'ui_static_' not in file_name:
            return
        #note this is including the original extension, so the zip file will have 2 extensions
        #this is on purpose for some reason
        output_file_names = [file_record.name]
    inputf = tempfile.NamedTemporaryFile(mode='wb', delete=False)
    inputf.write(file_record.read())
    inputf.flush()
    inputf.close()
    try:
        outputf = tempfile.NamedTemporaryFile(mode='wb', delete=False)
        outputf.close()
        try:
            song = AudioSegment.from_file(inputf.name,extension[1:])
            song = song.set_channels(1)
            song = song.set_frame_rate(22050)
            song.export(outputf.name,'s16le',codec = 'pcm_s16le')
            for output_file_name in output_file_names:
                target = output_file_name + '.snd'
                try:
                    essential_sounds.remove(target)
                except KeyError:
                    pass
                writeToZip(output_zip,outputf.name,os.path.join('sounds',target))
        finally:
            os.unlink(outputf.name)
    finally:
        os.unlink(inputf.name)

found_font = False
def HandleFnt(file_name,extension,file_record,output_zip):
    global found_font
    if 'monofonto_verylarge02_dialogs2' not in file_name:
        return
    writeStrToZip(output_zip,'monofonto_verylarge02_dialogs2.fnt',file_record.read())
    found_font = True

def AddCustomItems(zip_file):
    for filename in ('items_bg.png',
                     'data_bg.png',
                     'box.png',
                     'bar.png',
                     'band.png',
                     'stats_bg.png',
                     'chevrons.png',
                     'chevrons1.png',
                     'scanline.png',
                     'full.png',
                     'empty.png',
                     'fade.png'):
        zip_file.write(filename,filename)
    

if __name__ == '__main__':
    import sys
    import cStringIO as StringIO
    import Image
    import DDSImageFile
    import dds
    import zipfile
    import re
    from pydub import AudioSegment


    items_prefix = set(('weapons',
                        'vault_suit',
                        'item',
                        'items',
                        'apperal',
                        'appearal',
                        'apparel',
                        'missile'))

    handlers = {'.dds' : HandleDDS,
                '.mp3' : HandleMusic,
                '.ogg' : HandleMusic,
                '.wav' : HandleWav,
                '.fnt' : HandleFnt}

    if len(sys.argv) < 3:
        print 'Usage: %s output_dir [list of input dirs]' % sys.argv[0]
        raise SystemExit

    target_dir = sys.argv[1]
    if not os.path.isdir(target_dir):
        print '%s is not a directory' % sys.argv[1]
        raise SystemExit

    for filename in ('perks.txt','special.txt','skills.txt','items.txt'):
        #set up the default lists
        with open(filename,'rb') as inf:
            with open(os.path.join(target_dir,filename),'wb') as outf:
                outf.write(inf.read())

    #start the output zipfile
    with zipfile.ZipFile(os.path.join(target_dir,'data.zip'),'w') as output_zip:
        AddCustomItems(output_zip)
        for input_dir in sys.argv[2:]:
            for bsa_filename in glob.glob(os.path.join(input_dir,'*.bsa')):
                bsa = BSAFile(bsa_filename)
                print 'processing',bsa_filename
                #print bsa.version
                for file_record in bsa.file_records.values():
                    file_name,extension = os.path.splitext(file_record.name)
                    print file_name,extension,file_record.folder_name
                    continue
                    try:
                        handlers[extension](file_name,extension,file_record,output_zip)
                    except KeyError:
                        #print 'Ignoring file',file_name
                        pass

        for item in essential_dds:
            match = re.match('(perk|special|reputations|skills|weapons|vault_suit|item|items|apperal|appearal|apparel|missile)_.*\.dds',item)
            if match and match.groups()[0] in items_prefix:
                item = os.path.join('icons',item )
            if 'lincolnrifle' in item:
                if any('items_308_ammo' in t for t in output_zip.namelist()):
                    continue
            writeToZip(output_zip,'empty.png',os.path.splitext(item)[0] + '.png')

        for item in essential_sounds:
            writeStrToZip(output_zip,os.path.join('sounds',item),'\x00\x00')

    if not found_font:
        print 'Failed to find font, is your fallout installation complete?'
        raise SystemExit
        
    with open(os.path.join(target_dir,'music.txt'),'wb') as f:
        for song in music_files:
            f.write('{name}|{name}\n'.format(name = os.path.basename(song)))
