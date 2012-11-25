import sys,binascii

out = []

def bits(x):
    out = []
    for i in xrange(127,-1,-1):
        out.append((x>>i)&1)
    return out

all = 0xfffffffffffffffffffff7ffffffffff

counts = [0]*128


for line in sys.stdin:
    for word in line.split():
        word = int(word.strip(),16)
        for i,bit in enumerate(bits(word)):
            counts[i] += bit
        
for i,c in enumerate(counts):
    print i,c
    
#sys.stdout.write(''.join(out))
print '%.32x' % all

SkillsSubView::SkillsSubView() : selected_box(0.46,0.08,0.007) {
    pthread_mutex_init(&items_mutex,NULL);

    icon = new Image(DATA_DIR "reputations_novac.dimensions_256x256.raw",480./800,1.0,standard_tex_coords);
    if(icon == NULL)
        throw MEMORY_ERROR;

    items.push_back( PlacementInfo(0.17,0.8,1.4,1.4,new Text("Everyone",font)) );
    items.push_back( PlacementInfo(0.135,0.778,1.4,1.4,&selected_box) );
    items.push_back( PlacementInfo(0.17,0.8,1.4,1.4,icon) );

}
