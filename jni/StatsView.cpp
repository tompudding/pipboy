#include "gl_code.h"
#include <unistd.h>

StatsView::StatsView (const char *background_filename, Font *_font, GeneralConfig &config) {
    char temp_path[1024]      = {0};
    //Hideous memory leaks all over this function
    pthread_mutex_init(&items_mutex,NULL);
    background = new Image(background_filename,0.8,1,standard_tex_coords);
    font = _font;
    items.push_back( PlacementInfo(0.178,0.9450,2.,2.,new Text("STATS",font)) );

    if(sizeof(temp_path) <= snprintf(temp_path,sizeof(temp_path),"LVL %d",config.level)) {
        throw ErrorMessage(MEMORY_ERROR,"snprintf error in StatsView::StatsView");
    }
    temp_path[sizeof(temp_path)-1] = 0;
    items.push_back( PlacementInfo(0.31,0.91,1.4,1.4,new Text((const char *)temp_path,font)) );

    if(sizeof(temp_path) <= snprintf(temp_path,sizeof(temp_path),"HP %d/%d",config.hp.current,config.hp.max)) {
        throw ErrorMessage(MEMORY_ERROR,"snprintf error in StatsView::StatsView");
    }
    temp_path[sizeof(temp_path)-1] = 0;
    items.push_back( PlacementInfo(0.405,0.91,1.4,1.4,new Text((const char *)temp_path,font)) );

    if(sizeof(temp_path) <= snprintf(temp_path,sizeof(temp_path),"AP %d/%d",config.ap.current,config.ap.max)) {
        throw ErrorMessage(MEMORY_ERROR,"snprintf error in StatsView::StatsView");
    }
    temp_path[sizeof(temp_path)-1] = 0;
    items.push_back( PlacementInfo(0.566,0.91,1.4,1.4,new Text((const char *)temp_path,font)) );

    if(sizeof(temp_path) <= snprintf(temp_path,sizeof(temp_path),"XP %d/%d",config.xp.current,config.xp.max)) {
        throw ErrorMessage(MEMORY_ERROR,"snprintf error in StatsView::StatsView");
    }
    temp_path[sizeof(temp_path)-1] = 0;
    items.push_back( PlacementInfo(0.696,0.91,1.4,1.4,new Text((const char *)temp_path,font)) );
    items.push_back( PlacementInfo(0.142,0.031,1.4,1.4,new Text("Status",font)) );
    items.push_back( PlacementInfo(0.3030,0.031,1.4,1.4,new Text("S.P.E.C.I.A.L",font)) );
    items.push_back( PlacementInfo(0.49,0.031,1.4,1.4,new Text("Skills",font)) );
    items.push_back( PlacementInfo(0.631,0.031,1.4,1.4,new Text("Perks",font)) );
    items.push_back( PlacementInfo(0.757,0.031,1.4,1.4,new Text("General",font)) );

    boxes[0] = new PlacementInfo(0.132,0.005,1,1, new Box(0.16,0.08,0.007));
    boxes[1] = new PlacementInfo(0.288,0.005,1,1, new Box(0.247,0.08,0.007));
    boxes[2] = new PlacementInfo(0.469,0.005,1,1, new Box(0.176,0.08,0.007));
    boxes[3] = new PlacementInfo(0.618,0.005,1,1, new Box(0.154,0.08,0.007));
    boxes[4] = new PlacementInfo(0.749,0.005,1,1, new Box(0.175,0.08,0.007));

    subviews[0] = new StatusSubView(config);
    subviews[1] = new SpecialSubView();
    subviews[2] = new SkillsSubView();
    subviews[3] = new PerksSubView();
    subviews[4] = new GeneralSubView();

    for(int i=0;i<5;i++)
        if(subviews[i] == NULL || boxes[i] == NULL) { //fixme: memory leak 
            throw ErrorMessage(MEMORY_ERROR,"Memory Error initialising statsview");
        }

    current_view = 0;

    current_item = items.begin();
}

void StatsView::Up() {
    subviews[current_view]->Up();
}

void StatsView::Down() {
    subviews[current_view]->Down();
}

void StatsView::VerticalPos(float pos,int sound)
{
    subviews[current_view]->VerticalPos(pos,sound);
}

void StatsView::Right() {
    pthread_mutex_lock(&items_mutex);
    PlayClip(*menu_tab);
    current_view = (current_view + 1)%5;
    pthread_mutex_unlock(&items_mutex);
}

void StatsView::SetRotary(uint32_t value,float pos,uint32_t sound) {
    pthread_mutex_lock(&items_mutex);
    if(current_view != value) {
        if(sound)
            PlayClip(*menu_tab);
        current_view = value%5;
    }
    subviews[current_view]->VerticalPos(pos,0);
    pthread_mutex_unlock(&items_mutex);
}

void StatsView::Left() {
    
    pthread_mutex_lock(&items_mutex);
    PlayClip(*menu_tab);
    current_view = (current_view + 4)%5;
    pthread_mutex_unlock(&items_mutex);
}

void StatsView::Select() {
    
    pthread_mutex_lock(&items_mutex);
    subviews[current_view]->Select();
    pthread_mutex_unlock(&items_mutex);
}


void StatusSubView::Down() {
    pthread_mutex_lock(&items_mutex);
    PlayClip(*select_sound);
    current_view = (current_view + 1)%6;
    pthread_mutex_unlock(&items_mutex);
}

void StatusSubView::Up() {
    pthread_mutex_lock(&items_mutex);
    PlayClip(*select_sound);
    current_view = (current_view + 5)%6;
    pthread_mutex_unlock(&items_mutex);
}

void StatusSubView::VerticalPos(float pos,int sound) {
    int new_item = ((int)(pos*6))%6; 
    if(new_item != current_view) {
        pthread_mutex_lock(&items_mutex);
        if(sound)
            PlayClip(*select_sound);
        current_view = new_item;
        pthread_mutex_unlock(&items_mutex);
    }
}

StatusSubView::StatusSubView(GeneralConfig &config) {
    pthread_mutex_init(&items_mutex,NULL);
   
    //items.push_back( PlacementInfo(0.1335,0.779,1,1, new Box(0.13,0.08,0.007)));
    current_view = 0;
    
    items.push_back( PlacementInfo(0.15,0.8,1.6,1.6, new Text("CND",font)));
    items.push_back( PlacementInfo(0.15,0.712,1.6,1.6, new Text("RAD",font)));
    items.push_back( PlacementInfo(0.15,0.624,1.6,1.6, new Text("EFF",font)));
    items.push_back( PlacementInfo(0.15,0.536,1.6,1.6, new Text("H20",font)));
    items.push_back( PlacementInfo(0.15,0.448,1.6,1.6, new Text("FOD",font)));
    items.push_back( PlacementInfo(0.15,0.360,1.6,1.6, new Text("SLP",font)));

    GLfloat spacing = 0.088;
    for(int i=0;i<6;i++){
        boxes[i] = new PlacementInfo(0.1335,0.779-i*spacing,1,1, new Box(0.13,0.08,0.007));
    }

    subviews[0] = new ConditionSubView(config);
    subviews[1] = new RadiationSubView();
    subviews[2] = new EffectsSubView();
    subviews[3] = new WaterSubView();
    subviews[4] = new FoodSubView();
    subviews[5] = new SleepSubView();

}

ConditionSubView::ConditionSubView(GeneralConfig &config) {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.0,0.027,1.2,1.2,new Character(config)) );
    items.push_back( PlacementInfo(0.463,0.844,1,1, new ConditionBar(0.5,cb_plain)) );
    items.push_back( PlacementInfo(0.463,0.544,1,1, new ConditionBar(0.5,cb_plain)) );
    items.push_back( PlacementInfo(0.640,0.694,1,1, new ConditionBar(0.5,cb_pointleft)) );
    items.push_back( PlacementInfo(0.280,0.694,1,1, new ConditionBar(0.5,cb_pointright)) );
    items.push_back( PlacementInfo(0.640,0.294,1,1, new ConditionBar(0.5,cb_pointleft)) );
    items.push_back( PlacementInfo(0.280,0.294,1,1, new ConditionBar(0.5,cb_pointright)) );
    items.push_back( PlacementInfo(0.736,0.79,1.4,1.4, new Text("(20) Stimpak A)",font)));
    items.push_back( PlacementInfo(0.82,0.731,1.4,1.4, new Text("Limbs X)",font)));

}

RadiationSubView::RadiationSubView() {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.12,0.22,1.2,1.2,new HalfBox(0.4,0.08,0.007)) );
    items.push_back( PlacementInfo(0.38,0.22,1.2,1.2,new HalfBox(0.84,0.08,0.007)) );
    items.push_back( PlacementInfo(0.12,0.24,1.4,1.4, new Text("RAD RESIST     28%  RADS",font)));
    items.push_back( PlacementInfo(0.6,0.31,1.4,1.4, new Text("500                1000",font)));
    //items.push_back( PlacementInfo(0.4,0.24,1.0,1.0, arrow));
    for(int i=0;i<16;i++)
        items.push_back( PlacementInfo(0.52+i*0.023,0.28-((i%4)==0 ? 0.02 : 0),1,1,new Box(0,((i%4)==0 ? 0.04 : 0.02),0.002)) );
}

EffectsSubView::EffectsSubView() {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.26,0.55,1.8,1.8, new Text("Four Eyes         PER +1",font)));
    items.push_back( PlacementInfo(0.26,0.54,1,1,new HalfBox(0.84,0,0.005)) );
}

WaterSubView::WaterSubView() {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.12,0.22,1.2,1.2,new HalfBox(0.4,0.08,0.007)) );
    items.push_back( PlacementInfo(0.38,0.22,1.2,1.2,new HalfBox(0.84,0.08,0.007)) );
    items.push_back( PlacementInfo(0.12,0.24,1.4,1.4, new Text("                     H20",font)));
    items.push_back( PlacementInfo(0.6,0.31,1.4,1.4, new Text("500                1000",font)));
    for(int i=0;i<16;i++)
        items.push_back( PlacementInfo(0.52+i*0.023,0.28-((i%4)==0 ? 0.02 : 0),1,1,new Box(0,((i%4)==0 ? 0.04 : 0.02),0.002)) );
}

FoodSubView::FoodSubView() {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.12,0.22,1.2,1.2,new HalfBox(0.4,0.08,0.007)) );
    items.push_back( PlacementInfo(0.38,0.22,1.2,1.2,new HalfBox(0.84,0.08,0.007)) );
    items.push_back( PlacementInfo(0.12,0.24,1.4,1.4, new Text("                     FOD",font)));
    items.push_back( PlacementInfo(0.6,0.31,1.4,1.4, new Text("500                1000",font)));
    for(int i=0;i<16;i++)
        items.push_back( PlacementInfo(0.52+i*0.023,0.28-((i%4)==0 ? 0.02 : 0),1,1,new Box(0,((i%4)==0 ? 0.04 : 0.02),0.002)) );
}

SleepSubView::SleepSubView() {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.12,0.22,1.2,1.2,new HalfBox(0.4,0.08,0.007)) );
    items.push_back( PlacementInfo(0.38,0.22,1.2,1.2,new HalfBox(0.84,0.08,0.007)) );
    items.push_back( PlacementInfo(0.12,0.24,1.4,1.4, new Text("                     SLP",font)));
    items.push_back( PlacementInfo(0.6,0.31,1.4,1.4, new Text("500                1000",font)));
    for(int i=0;i<16;i++)
        items.push_back( PlacementInfo(0.52+i*0.023,0.28-((i%4)==0 ? 0.02 : 0),1,1,new Box(0,((i%4)==0 ? 0.04 : 0.02),0.002)) );
}

SpecialSubView::SpecialSubView() : selected_box(0.42,0.08,0.007),box(0.78,0.08,0.007) {
    pthread_mutex_init(&items_mutex,NULL);
    char buffer[1024] = {0};
    const char *temp_desc[7][5] = { {"Strength is a measure of your raw",
                                     "physical power. It affects how much you",
                                     "can carry, the power of all melee",
                                     "attacks, and your effectiveness with",
                                     "many heavy weapons"},
                                    {"A high perception grants a bonus to",
                                     "the Explosives, Lockpick and Energy",
                                     "Weapons skills, and determines when",
                                     "red compass markings appear (which",
                                     "indicate threats)"},
                                    {"Endurance is a measure of your overall",
                                     "physical fitness. A high Endurance",
                                     "gives bonuses to health, environmental",
                                     "resistances, and the Survival and",
                                     "Unarmed skills"},
                                    {"Having a high charisma will improve",
                                     "people's disposition towards you, and",
                                     "give bonuses to both the Barter and",
                                     "Speech skills",""},
                                    {"Intelligence affects the Science,",
                                     "Repair and Medicine skills. The higher",
                                     "your Intelligence, the more Skill",
                                     "Points you'll be able to distribute when",
                                     "you level up"},
                                    {"Agility affects your Guns and Sneak",
                                     "skills, and the number of Action Points",
                                     "available for V.A.T.S","",""},
                                    {"Raising your Luck will raise all of your",
                                     "skills a little. Having a high Luck will",
                                     "also improve your critical chance with",
                                     "all weapons.",""}};

    FILE *f;
    error result = OK;
    int i;
    string error_message;

    memcpy(descriptions,temp_desc,sizeof(descriptions));
    f = fopen(DATA_DIR "special.txt","rb");
    if(NULL == f) {
        result = FILE_NOT_FOUND;
        error_message = "Error opening ";
        error_message += DATA_DIR "special.txt";
        goto finish;
    }
    for(i=0;i<7;i++) {
        if(NULL == fgets(buffer,sizeof(buffer),f)) {
            //file ran out before we got all the data
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "special.txt";
            goto close_file;
        }
        stats[i] = strtoul(buffer,NULL,10);
        if(stats[i] < 0) {
            stats[i] = 0;
        }
        if(stats[i] > 10) {
            stats[i] = 10;
        }
    }
    
    names[0] = "Strength";
    names[1] = "Perception";
    names[2] = "Endurance";
    names[3] = "Charisma";
    names[4] = "Intelligence";
    names[5] = "Agility";
    names[6] = "Luck";


    icons[0] = new Image("special_strength.png",480./800,1.0,standard_tex_coords);
    icons[1] = new Image("special_perception.png",480./800,1.0,standard_tex_coords);
    icons[2] = new Image("special_endurance.png",480./800,1.0,standard_tex_coords);
    icons[3] = new Image("special_charisma.png",480./800,1.0,standard_tex_coords);
    icons[4] = new Image("special_intelligence.png",480./800,1.0,standard_tex_coords);
    icons[5] = new Image("special_agility.png",480./800,1.0,standard_tex_coords);
    icons[6] = new Image("special_luck.png",480./800,1.0,standard_tex_coords);

    for(int i=0;i<7;i++)
    {
        Text *t = new Text(names[i],font);
        if(NULL == t) {
            result = MEMORY_ERROR;
            error_message = "memory_error";
            goto cleanup;
        }
        items.push_back( PlacementInfo(0.17,0.8-0.08*i,1.4,1.4,t) );
    }

    for(int i=0;i<7;i++)
        if(icons[i] == NULL) {
            result = MEMORY_ERROR;
            error_message = "memory_error";
            goto cleanup;
        }

    current_item = 0;

cleanup:
    //We don't free the texts, so this is a memory leak. fix in the future if I can be bothered
    if(OK != result) {
        for(i=0;i<7;i++) {
            if(NULL != icons[i]) {
                delete icons[i];
            }
        }
    }

close_file:
    if(NULL != f) {
        fclose(f);
    }

finish:
    if(result != OK) {
        throw ErrorMessage(result,error_message);
    }
    
}

SkillsSubView::SkillsSubView() : selected_box(0.46,0.08,0.007),box(0.78,0.08,0.007) {
    pthread_mutex_init(&items_mutex,NULL);
    const char *temp_desc[13][5] = { {"The Barter skill affects the prices",
                                     "you get for buying and selling",
                                     "items. In general, the higher your",
                                     "Barter skill, the lower your prices",
                                     "on purchased items"},
                                    {"The Energy Weapons skill",
                                     "determines your effectiveness",
                                     "with any weapon that uses Small",
                                     "Energy Cells, Micro Fusion Cells, EC",
                                     "Packs, or Flamer Fuel"},
                                     {"The Explosives skill determines",
                                     "the ease of disarming any hostile",
                                     "mines and the effectiveness of any",
                                     "explosive weapon.",
                                     ""},
                                     {"Guns determines your",
                                     "effectiveness with any weapon",
                                     "that uses conventional ammunition",
                                     "(.22 KR, .357 Magnum,5mm,10mm,",
                                     "5.56mm,.308,.45-70 Gov't etc.)."},
                                     {"The Lockpick skill is used to open",
                                     "locked doors and containers",
                                     "",
                                     "",
                                     ""},
                                     {"The Medicine skill determines how",
                                     "many Hit Points you'll replenish",
                                     "upon using a Stimpak, and the",
                                     "effectiveness of Rad-X and",
                                     "RadAway"},
                                     {"The Melee Weapons skill",
                                     "determines your effectiveness",
                                     "with any melee weapon, from the",
                                     "simple lead pipe all the way up to",
                                     "the high-tech Super Sledge"},
                                     {"The Repair skill allows you to",
                                     "maintain any weapons and apparel.",
                                     "In addition, Repair allows you to",
                                     "create items and Guns ammunition",
                                     "at reloading benches"},
                                     {"The Science skill represents your",
                                     "combined scientific knowledge,",
                                     "and is primarily used to hack",
                                     "restricted computer terminals.",
                                     ""},
                                     {"The higher your Sneak skill, the",
                                     "easier it is to remain undetected,",
                                     "steal an item or pick someone's",
                                     "pocket.",
                                     ""},
                                     {"The Speech skill governs how much",
                                     "you can influence someone through",
                                     "dialogue, and gain access to",
                                     "information they might otherwise",
                                     "not want to share."},
                                     {"The Survival skill increases the Hit",
                                     "Points you receive from food and",
                                     "drink. It also helps you create",
                                     "consumable items at campfires.",
                                     ""},
                                     {"The Unarmed skill is used for",
                                     "fighting without a weapon, or with",
                                     "weapons designed for",
                                     "hand-to-hand combat, like Brass",
                                      "knuckles and Power Fists."}};
    //uint32_t temp_stats[13] = {35,0,0,3,5,7,25,40,81,27,40,23,20};
    const char *temp_names[13] = {"Barter",
                                  "Energy Weapons",
                                  "Explosives",
                                  "Guns",
                                  "Lockpick",
                                  "Medicine",
                                  "Melee Weapons",
                                  "Repair",
                                  "Science",
                                  "Sneak",
                                  "Speech",
                                  "Survival",
                                  "Unarmed"};
    FILE *f;
    error result = OK;
    char buffer[1024] = {0};
    int i;
    string error_message;

    display_start = 0;
    display_num   = 8;
    current_item  = 0;

    memcpy(descriptions,temp_desc,sizeof(descriptions));
    memcpy(names,temp_names,sizeof(names));

    f = fopen(DATA_DIR "skills.txt","rb");
    if(NULL == f) {
        result = FILE_NOT_FOUND;
        error_message = "Error opening ";
        error_message += DATA_DIR "skills.txt";
        goto finish;
    }
    for(i=0;i<13;i++) {
        if(NULL == fgets(buffer,sizeof(buffer),f)) {
            //file ran out before we got all the data
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "skills.txt";
            goto close_file;
        }
        stats[i] = strtoul(buffer,NULL,10);
        if(stats[i] < 0) {
            stats[i] = 0;
        }
        if(stats[i] > 100) {
            stats[i] = 100;
        }
    }

    icons[ 0] = new Image("skills_barter.png",480./800,1.0,standard_tex_coords);
    icons[ 1] = new Image("skills_energy_weapons.png",480./800,1.0,standard_tex_coords);
    icons[ 2] = new Image("skills_explosives.png",480./800,1.0,standard_tex_coords);
    icons[ 3] = new Image("skills_small_guns.png",480./800,1.0,standard_tex_coords);
    icons[ 4] = new Image("skills_lockpick.png",480./800,1.0,standard_tex_coords);
    icons[ 5] = new Image("skills_medicine.png",480./800,1.0,standard_tex_coords);
    icons[ 6] = new Image("skills_melee_weapons.png",480./800,1.0,standard_tex_coords);
    icons[ 7] = new Image("skills_repair.png",480./800,1.0,standard_tex_coords);
    icons[ 8] = new Image("skills_science.png",480./800,1.0,standard_tex_coords);
    icons[ 9] = new Image("skills_sneak.png",480./800,1.0,standard_tex_coords);
    icons[10] = new Image("skills_speech.png",480./800,1.0,standard_tex_coords);
    icons[11] = new Image("skills_survival.png",480./800,1.0,standard_tex_coords);
    icons[12] = new Image("skills_unarmed.png",480./800,1.0,standard_tex_coords);

    for(int i=0;i<13;i++)
        items.push_back( PlacementInfo(0.17,0.8-0.08*i,1.4,1.4,new Text(names[i],font)) );

    for(int i=0;i<13;i++)
        if(icons[i] == NULL) {
            result = MEMORY_ERROR; //memory leak fixme
            error_message = "memory_error";
            goto cleanup;
        }

    scrollbar = new ScrollBar();
    if(scrollbar == NULL) {
        result = MEMORY_ERROR;
        error_message = "memory_error";
        goto cleanup;
    }
    scrollbar->SetData(items.size(),display_start,display_num);

cleanup:
    //We don't free the texts, so this is a memory leak. fix in the future if I can be bothered
    if(OK != result) {
        for(i=0;i<13;i++) {
            if(NULL != icons[i]) {
                delete icons[i];
            }
        }
    }

close_file:
    if(NULL != f) {
        fclose(f);
    }

finish:
    if(result != OK) {
        throw ErrorMessage(result,error_message);
    }

}

PerksSubView::PerksSubView() : selected_box(0.46,0.08,0.007),box(0.78,0.08,0.007){
    pthread_mutex_init(&items_mutex,NULL);
    int i,j;
    FILE *f;
    char buffer[1024] = {0};
    error result = OK;
    icons = NULL;
    names = NULL;
    descriptions = NULL;
    string error_message;

    display_start = 0;
    display_num   = 8;
    current_item  = 0;
    num_perks     = 0;

    f = fopen(DATA_DIR "perks.txt","rb");
    if(NULL == f) {
        result = FILE_NOT_FOUND;
        error_message = "Error opening ";
        error_message += DATA_DIR "perks.txt";
        goto finish;
    }
    while(NULL != fgets(buffer,sizeof(buffer),f)) {
    //do a first pass to establish how many perks we're doing
        if(buffer[0] == '\0' || buffer[0] == '#' || buffer[0] == '\n') {
            continue;
        }
        num_perks++;
    }

    LOGI("perks*** : Have %d perks",num_perks);

    icons = (Image**)malloc(num_perks*sizeof(Image*));
    if(NULL == icons) {
        result = MEMORY_ERROR;
        error_message = "memory_error";
        goto cleanup;
    }
    memset(icons,0,num_perks*sizeof(Image*));
    names = (const char**)malloc(num_perks*sizeof(char*));
    if(NULL == names) {
        result = MEMORY_ERROR;
        error_message = "memory_error";
        goto cleanup;
    }
    memset(icons,0,num_perks*sizeof(char*));
    descriptions = (const char***)malloc(num_perks*sizeof(char**));
    if(NULL == descriptions) {
        result = MEMORY_ERROR;
        error_message = "memory_error";
        goto cleanup;
    }
    memset(descriptions,0,num_perks*sizeof(char**));
    for(i=0;i<num_perks;i++) {
        //Magic numbers, yay!
        descriptions[i] = (const char**)malloc(5*sizeof(char*));
        if(NULL == descriptions[i]) {
            result = MEMORY_ERROR;
            error_message = "memory_error";
            goto cleanup;
        }
        for(j=0;j<5;j++) {
            descriptions[i][j] = NULL;
        }
    }

    fseek(f,0,SEEK_SET);
    i = 0;
    while(NULL != fgets(buffer,sizeof(buffer),f)) {
    //do a first pass to establish how many perks we're doing
        char *saveptr;
        if(buffer[0] == '\0' || buffer[0] == '#' || buffer[0] == '\n') {
            continue;
        }
        //strtok is a well known terrible function, but it's all I've got.
        names[i] = strtok_r(buffer,"|",&saveptr);
        if(NULL == names[i]) {
            ostringstream oss;
            oss << num_perks;
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "perks.txt";
            error_message += "On perk";
            error_message += oss.str();
            goto cleanup;
        }
        LOGI("perks*** : Perk[%d] : %s",i,names[i]);
        //names[i] is pointing to a stack buffer, need a copy of it before this function leaves...
        names[i] = strdup(names[i]);
        char *filename = strtok_r(NULL,"|",&saveptr);
        if(NULL == filename) {
            ostringstream oss;
            oss << num_perks;
            result = FILE_ERROR;
            error_message = "Error parsing ";
            error_message += DATA_DIR "perks.txt";
            error_message += "On perk";
            error_message += oss.str();
            goto cleanup;
        }
        icons[i] = new Image(filename,480./800,1.0,standard_tex_coords);
        if(NULL == icons[i]) {
            result = MEMORY_ERROR;
            error_message = "memory_error";
            goto cleanup;
        }
        for(j=0;j<5;j++) {
            descriptions[i][j] = strtok_r(NULL,"|",&saveptr);
            if(NULL == descriptions[i][j]) {
                descriptions[i][j] = strdup(" ");
            }
            else {
                for(int k=0;descriptions[i][j][k];k++) {
                    if(descriptions[i][j][k] == '\n') {
                        ((char*)(descriptions[i][j]))[k] = ' ';
                    }
                }
                descriptions[i][j] = strdup(descriptions[i][j]);
            }
            LOGI("perks*** : Perk[%d] : %s",i,descriptions[i][j]);
        }
        i++;
    }
    

    for(int i=0;i<num_perks;i++)
        items.push_back( PlacementInfo(0.17,0.8-0.08*i,1.4,1.4,new Text(names[i],font)) );

    scrollbar = new ScrollBar();
    if(scrollbar == NULL) {
        result = MEMORY_ERROR;
        error_message = "memory_error";
        goto cleanup;
    }
    scrollbar->SetData(items.size(),display_start,display_num);

cleanup:
    if(result != OK) {
        for(i=0;i<num_perks;i++) {
            if(NULL != names[i]) {
                free((void*)names[i]);
            }
            for(j=0;j<5;j++) {
                if(NULL != descriptions[i][j]) {
                    free((void*)descriptions[i][j]);
                }
            }
            if(NULL != icons[i]) {
                delete icons[i];
            }
        }
        if(NULL != icons) {
            free((void*)icons);
        }
        if(NULL != names) {
            free((void*)names);
        }
        if(NULL != descriptions) {
            free((void*)descriptions);
        }
    }

close_file:
    if(NULL != f) {
        fclose(f);
    }

finish:
    if(result != OK) {
        throw ErrorMessage(result,error_message);
    }
}

PerksSubView::~PerksSubView() {
    int i,j;
    for(i=0;i<num_perks;i++) {
        if(NULL != names[i]) {
            free((void*)names[i]);
        }
        for(j=0;j<5;j++) {
            if(NULL != descriptions[i][j]) {
                free((void*)descriptions[i][j]);
            }
        }
    }
    if(NULL != icons) {
        free((void*)icons);
    }
    if(NULL != names) {
        free((void*)names);
    }
    if(NULL != descriptions) {
        free((void*)descriptions);
    }
}

GeneralSubView::GeneralSubView() : selected_box(0.46,0.08,0.007) {
    pthread_mutex_init(&items_mutex,NULL);

    icon = new Image("reputations_novac.png",480./800,1.0,standard_tex_coords);
    if(icon == NULL)
        throw ErrorMessage(MEMORY_ERROR,"memory error");

    items.push_back( PlacementInfo(0.17,0.8,1.4,1.4,new Text("Everyone",font)) );
    items.push_back( PlacementInfo(0.135,0.778,1.4,1.4,&selected_box) );
    items.push_back( PlacementInfo(0.492,0.25,0.7,0.7,icon) );
    items.push_back( PlacementInfo(0.65,0.25,1.4,1.4,new Text("Idolized",font)) );

}



void View::NextItem() {
    pthread_mutex_lock(&items_mutex);
    current_item++;
    if(current_item == items.end())
        current_item = items.begin();
    pthread_mutex_unlock(&items_mutex);
}

void View::Move(GLfloat x, GLfloat y) {
    pthread_mutex_lock(&items_mutex);
    current_item->x += x;
    current_item->y += y;
    LOGI("New item position is %.4f %.4f",current_item->x,current_item->y);
    pthread_mutex_unlock(&items_mutex);
}

void SpecialSubView::Down() {
    pthread_mutex_lock(&items_mutex);
    if(current_item != 6)
    {
        current_item++;
        PlayClip(*menu_prevnext);
    }
    pthread_mutex_unlock(&items_mutex);
}

void SpecialSubView::Up() {
    pthread_mutex_lock(&items_mutex);
    if(current_item != 0)
    {
        PlayClip(*menu_prevnext);
        current_item--;
    }
    pthread_mutex_unlock(&items_mutex);
}

void SpecialSubView::VerticalPos(float pos,int sound) {
    int new_item = ((int)(pos*7))%7; 
    if(new_item != current_item) {
        pthread_mutex_lock(&items_mutex);
        if(sound)
            PlayClip(*menu_prevnext);
        current_item = new_item;
        pthread_mutex_unlock(&items_mutex);
    }
}

void SkillsSubView::Down() {
    pthread_mutex_lock(&items_mutex);
    if(current_item != 12)
    {
        current_item++;
        PlayClip(*menu_prevnext);
        if(current_item > display_start+display_num)
        {
            display_start++;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
    }
    pthread_mutex_unlock(&items_mutex);
}

void SkillsSubView::Up() {
    pthread_mutex_lock(&items_mutex);
    if(current_item != 0)
    {
        PlayClip(*menu_prevnext);
        current_item--;
        if((current_item+1) == display_start) {
            //they can't be zero or current_item would = itemlist.begin() //I hope!
            display_start--;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
    }
    pthread_mutex_unlock(&items_mutex);
}

void SkillsSubView::VerticalPos(float pos,int sound) {
    int new_item = ((int)(pos*items.size()))%items.size(); 
    if(new_item != current_item) {
        pthread_mutex_lock(&items_mutex);
        if(sound)
            PlayClip(*menu_prevnext);
        current_item = new_item;
        if(current_item > display_start+display_num)
        {
            display_start = current_item - display_num;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
        else if(current_item < display_start) {
            display_start = current_item;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
        pthread_mutex_unlock(&items_mutex);
    }
}

void PerksSubView::Down() {
    pthread_mutex_lock(&items_mutex);
    if(current_item != num_perks-1)
    {
        current_item++;
        PlayClip(*menu_prevnext);
        if(current_item > display_start+display_num)
        {
            display_start++;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
    }
    pthread_mutex_unlock(&items_mutex);
}

void PerksSubView::Up() {
    pthread_mutex_lock(&items_mutex);
    if(current_item != 0)
    {
        PlayClip(*menu_prevnext);
        current_item--;
        if((current_item+1) == display_start) {
            //they can't be zero or current_item would = itemlist.begin() //I hope!
            display_start--;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
    }
    pthread_mutex_unlock(&items_mutex);
}

void PerksSubView::VerticalPos(float pos,int sound) {
    int new_item = ((int)(pos*items.size()))%items.size(); 
    if(new_item != current_item) {
        pthread_mutex_lock(&items_mutex);
        if(sound)
            PlayClip(*menu_prevnext);
        current_item = new_item;
        if(current_item > display_start+display_num)
        {
            display_start = current_item - display_num;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
        else if(current_item < display_start) {
            display_start = current_item;
            scrollbar->SetData(items.size(),display_start,display_num);
        }
        pthread_mutex_unlock(&items_mutex);
   } 
}

void StatsView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    background->Draw(x,y,xscale,yscale);

    pthread_mutex_lock(&items_mutex);
    for(items_t::iterator i=items.begin(); i != items.end();i++) {
        i->item->Draw(i->x,i->y,i->xscale,i->yscale);
    }
    subviews[current_view]->Draw(0,0,1,1);
    int i = current_view;
    boxes[i]->item->Draw(boxes[i]->x,boxes[i]->y,boxes[i]->xscale,boxes[i]->yscale);
    pthread_mutex_unlock(&items_mutex);
}

void StatusSubView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    pthread_mutex_lock(&items_mutex);
    for(items_t::iterator i=items.begin(); i != items.end();i++) {
        i->item->Draw(i->x,i->y,i->xscale,i->yscale);
    }
    if(subviews[current_view] != NULL)
        subviews[current_view]->Draw(0,0,1,1);
    int i = current_view;
    boxes[i]->item->Draw(boxes[i]->x,boxes[i]->y,boxes[i]->xscale,boxes[i]->yscale);
    pthread_mutex_unlock(&items_mutex);
}


void View::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    pthread_mutex_lock(&items_mutex);
    for(items_t::iterator i=items.begin(); i != items.end();i++) {
        i->item->Draw(i->x,i->y,i->xscale,i->yscale);
    }
    pthread_mutex_unlock(&items_mutex);
}

void SpecialSubView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    pthread_mutex_lock(&items_mutex);
    for(items_t::iterator i=items.begin(); i != items.end();i++) {
        i->item->Draw(i->x,i->y,i->xscale,i->yscale);
    }

    for(int i=0;i<7;i++) {
        char temp[3] = {0};
        temp[0] = stats[i] >= 10 ? '1' : ' ';
        temp[1] = '0' + ((stats[i]%10));
        general_text->SetText((const char*)&temp);
        general_text->Draw(x+0.33,y+0.8-0.08*i,1.4,1.4);
    }
    selected_box.Draw(x+0.135,y+0.778-(current_item*0.08),1,1);
    icons[current_item]->Draw(x+0.492,y+0.33,0.65,0.65);
    box.Draw(x+0.415,y+0.3);
    for(int i=0;i<5;i++) {
        general_text->SetText((const char*)descriptions[current_item][i]);
        general_text->Draw(x+0.415,y+0.30-0.05*i,1.3,1.3);
    }
    pthread_mutex_unlock(&items_mutex);
}

void SkillsSubView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    pthread_mutex_lock(&items_mutex);
    int count = 0;
    for(items_t::iterator i=items.begin(); i != items.end();i++,count++) {
        int effective_count = count-display_start;
        char temp[6];
        if(count < display_start)
            continue;
        i->item->Draw(i->x,i->y+(display_start*0.08),i->xscale,i->yscale);
        snprintf(temp,sizeof(temp),"%3d",stats[count]);
        general_text->SetText((const char*)&temp);
        general_text->Draw(x+0.36,y+0.8-0.08*effective_count,1.4,1.4);

        if(count >= display_start + display_num)
            break;
    }

    selected_box.Draw(x+0.135,y+0.778-((current_item-display_start)*0.08),1,1);
    icons[current_item]->Draw(x+0.492,y+0.33,0.65,0.65);
    box.Draw(x+0.415,y+0.3);
    for(int i=0;i<5;i++) {
        general_text->SetText((const char*)descriptions[current_item][i]);
        general_text->Draw(x+0.415,y+0.30-0.05*i,1.3,1.3);
    }
    if(items.size() > display_num) {
        //draw the scroll bar
        //first the top arrow
        scrollbar->Draw(x,y,1,1);
    }
    pthread_mutex_unlock(&items_mutex);
}

void PerksSubView::Draw(GLfloat x, GLfloat y,GLfloat xscale, GLfloat yscale) {
    pthread_mutex_lock(&items_mutex);
    int count = 0;
    for(items_t::iterator i=items.begin(); i != items.end();i++,count++) {
        int effective_count = count-display_start;
        char temp[6];
        if(count < display_start)
            continue;
        i->item->Draw(i->x,i->y+(display_start*0.08),i->xscale,i->yscale);

        if(count >= display_start + display_num)
            break;
    }

    selected_box.Draw(x+0.135,y+0.778-((current_item-display_start)*0.08),1,1);
    icons[current_item]->Draw(x+0.492,y+0.33,0.65,0.65);
    box.Draw(x+0.415,y+0.3);
    for(int i=0;i<5;i++) {
        general_text->SetText((const char*)descriptions[current_item][i]);
        general_text->Draw(x+0.415,y+0.30-0.05*i,1.3,1.3);
    }
    if(items.size() > display_num) {
        //draw the scroll bar
        //first the top arrow
        scrollbar->Draw(x,y,1,1);
    }
    pthread_mutex_unlock(&items_mutex);
}


