#include "gl_code.h"
#include <unistd.h>

StatsView::StatsView (const char *background_filename, Font *_font) {
    pthread_mutex_init(&items_mutex,NULL);
    background = new Image(background_filename,0.8,1,standard_tex_coords);
    font = _font;
    items.push_back( PlacementInfo(0.178,0.9450,2.,2.,new Text("STATS",font)) );
    items.push_back( PlacementInfo(0.31,0.91,1.4,1.4,new Text("LVL 19",font)) );
    items.push_back( PlacementInfo(0.405,0.91,1.4,1.4,new Text("HP 460/460",font)) );
    items.push_back( PlacementInfo(0.566,0.91,1.4,1.4,new Text("AP 75/75",font)) );
    items.push_back( PlacementInfo(0.696,0.91,1.4,1.4,new Text("XP 29297/29450",font)) );
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

    subviews[0] = new StatusSubView();
    subviews[1] = new SpecialSubView();
    subviews[2] = new SkillsSubView();
    subviews[3] = new PerksSubView();
    subviews[4] = new GeneralSubView();

    for(int i=0;i<5;i++)
        if(subviews[i] == NULL || boxes[i] == NULL) //fixme: memory leak
            throw MEMORY_ERROR;

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

StatusSubView::StatusSubView() {
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

    subviews[0] = new ConditionSubView();
    subviews[1] = new RadiationSubView();
    subviews[2] = new EffectsSubView();
    subviews[3] = new WaterSubView();
    subviews[4] = new FoodSubView();
    subviews[5] = new SleepSubView();

}

ConditionSubView::ConditionSubView() {
    pthread_mutex_init(&items_mutex,NULL);
    items.push_back( PlacementInfo(0.0,0.027,1.2,1.2,new Character()) );
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

    memcpy(descriptions,temp_desc,sizeof(descriptions));
    stats[0] = 5;
    stats[1] = 3;
    stats[2] = 4;
    stats[3] = 4;
    stats[4] = 7;
    stats[5] = 5;
    stats[6] = 5;
    
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
            throw MEMORY_ERROR;
        }
        items.push_back( PlacementInfo(0.17,0.8-0.08*i,1.4,1.4,t) );
    }

    for(int i=0;i<7;i++)
        if(icons[i] == NULL)
            throw MEMORY_ERROR; //memory leak fixme

    current_item = 0;
    
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
    uint32_t temp_stats[13] = {35,0,0,3,5,7,25,40,81,27,40,23,20};
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

    display_start = 0;
    display_num   = 8;
    current_item  = 0;

    memcpy(descriptions,temp_desc,sizeof(descriptions));
    memcpy(stats,temp_stats,sizeof(stats));
    memcpy(names,temp_names,sizeof(names));

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
        if(icons[i] == NULL)
            throw MEMORY_ERROR; //memory leak fixme

    scrollbar = new ScrollBar();
    if(scrollbar == NULL)
        throw MEMORY_ERROR;
    scrollbar->SetData(items.size(),display_start,display_num);
    
}

PerksSubView::PerksSubView() : selected_box(0.46,0.08,0.007),box(0.78,0.08,0.007){
    pthread_mutex_init(&items_mutex,NULL);
    const char *temp_desc[13][5] = { {"Your beard strikes fear into the",
                                      "heart of your opponents, and ",
                                      "opens up new dialogue options. It",
                                      "also absorbs 90% of all damage",
                                      ""},
                                     {"All know of your legendary",
                                      "awesomeness. The higher an enemy's",
                                      "intelligence, the more likely they",
                                      "are to run away from you",
                                      ""},
                                     {"Chems last twice as long",
                                      "",
                                      "",
                                      "",
                                      ""},
                                     {"You gain one additional skill",
                                      "point for reading books and double",
                                      "the skill points for reading",
                                      "magazines.",
                                      ""},
                                     {"Can hack a locked down ",
                                      "terminal with four more",
                                      "chances.",
                                      "",
                                      ""},
                                     {"Costumes with electronic",
                                      "components can be constructed",
                                      "200% faster",
                                      "",
                                      ""},
                                     {"With the Educated perk, you gain",
                                      "two more skill points every time",
                                      "you advance in level. This perk is",
                                      "best taken early on, to maximise",
                                      "its effectiveness"},
                                     {"While wearing any type of",
                                      "glasses, you have +1 PER. Without",
                                      "glasses, you have -1 PER",
                                      "",
                                      ""},
                                     {"You are able to optimize your",
                                      "Pip-Boy's V.A.T.S logic, reducing",
                                      "all AP costs by 10%",
                                      "",
                                      ""},
                                     {"+15 DT and Strength increased to",
                                      "10 whenever health is 20% or",
                                      "lower.",
                                      "",
                                      ""},
                                     {"With each rank in the Swift",
                                      "Learner perk, you gain an",
                                      "additional 10% to toal",
                                      "Experience Points whenever",
                                      "Experience Points are earned."},
                                     {"While wearing light armor or",
                                      "no armor, you run 10% faster.",
                                      "",
                                      "",
                                      ""},
                                     {"Numerous prestigious awards",
                                      "have given you spectacular",
                                      "superpowers. Speech is set",
                                      "to 100% for interlocutors with",
                                      "intelligence less than 4."},
    };
    const char *temp_names[13] = {"Amazing Beard",
                                  "Captain Awesome",
                                  "Chemist",
                                  "Comprehension",
                                  "Computer Whizz",
                                  "Costume Guy",
                                  "Educated",
                                  "Four Eyes",
                                  "Math Wrath",
                                  "Nerd Rage",
                                  "Swift Learner",
                                  "Travel Light",
                                  "The Award Winning"};
    int i,j;

    display_start = 0;
    display_num   = 8;
    current_item  = 0;
    num_perks     = 13;

    //There's all kinds of memory leaks here. This is what you get for using C++ in a weird C
    //type fashing where you don't use objects for things. Sigh.
    icons = (Image**)malloc(num_perks*sizeof(Image*));
    if(NULL == icons) {
        throw MEMORY_ERROR;
    }
    names = (const char**)malloc(num_perks*sizeof(char*));
    if(NULL == names) {
        throw MEMORY_ERROR;
    }
    descriptions = (const char***)malloc(num_perks*sizeof(char**));
    if(NULL == descriptions) {
        throw MEMORY_ERROR;
    }
    for(i=0;i<num_perks;i++) {
        //Magic numbers, yay!
        descriptions[i] = (const char**)malloc(5*sizeof(char*));
        if(NULL == descriptions[i]) {
            throw MEMORY_ERROR;
        }
        for(j=0;j<5;j++) {
            descriptions[i][j] = temp_desc[i][j];
        }
    }

    for(i=0;i<num_perks;i++) {
        names[i] = temp_names[i];
    }

    icons[ 0] = new Image("perk_life_giver.png",480./800,1.0,standard_tex_coords);
    icons[ 1] = new Image("perk_action_boy.png",480./800,1.0,standard_tex_coords);
    icons[ 2] = new Image("perk_chemist.png",480./800,1.0,standard_tex_coords);
    icons[ 3] = new Image("perk_comprehension.png",480./800,1.0,standard_tex_coords);
    icons[ 4] = new Image("perk_l33t_haxx0r.png",480./800,1.0,standard_tex_coords);
    icons[ 5] = new Image("perk_robotics_expert.png",480./800,1.0,standard_tex_coords);
    icons[ 6] = new Image("perk_educated.png",480./800,1.0,standard_tex_coords);
    icons[ 7] = new Image("perk_four_eyes.png",480./800,1.0,standard_tex_coords);
    icons[ 8] = new Image("perk_math_wrath.png",480./800,1.0,standard_tex_coords);
    icons[ 9] = new Image("perk_nerd_rage!.png",480./800,1.0,standard_tex_coords);
    icons[10] = new Image("perk_swift_learner.png",480./800,1.0,standard_tex_coords);
    icons[11] = new Image("perk_travel_light.png",480./800,1.0,standard_tex_coords);
    icons[12] = new Image("perk_challenge.png",480./800,1.0,standard_tex_coords);

    for(int i=0;i<num_perks;i++)
        items.push_back( PlacementInfo(0.17,0.8-0.08*i,1.4,1.4,new Text(names[i],font)) );

    for(int i=0;i<num_perks;i++)
        if(icons[i] == NULL)
            throw MEMORY_ERROR; //memory leak fixme

    scrollbar = new ScrollBar();
    if(scrollbar == NULL)
        throw MEMORY_ERROR;
    scrollbar->SetData(items.size(),display_start,display_num);
}

GeneralSubView::GeneralSubView() : selected_box(0.46,0.08,0.007) {
    pthread_mutex_init(&items_mutex,NULL);

    icon = new Image("reputations_novac.png",480./800,1.0,standard_tex_coords);
    if(icon == NULL)
        throw MEMORY_ERROR;

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
        char temp[2] = {0};
        temp[0] = '0' + ((stats[i]%10));
        general_text->text = (const char*)&temp;
        general_text->Draw(x+0.33,y+0.8-0.08*i,1.4,1.4);
    }
    selected_box.Draw(x+0.135,y+0.778-(current_item*0.08),1,1);
    icons[current_item]->Draw(x+0.492,y+0.33,0.65,0.65);
    box.Draw(x+0.415,y+0.3);
    for(int i=0;i<5;i++) {
        general_text->text = descriptions[current_item][i];
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
        general_text->text = (const char*)&temp;
        general_text->Draw(x+0.36,y+0.8-0.08*effective_count,1.4,1.4);

        if(count >= display_start + display_num)
            break;
    }

    selected_box.Draw(x+0.135,y+0.778-((current_item-display_start)*0.08),1,1);
    icons[current_item]->Draw(x+0.492,y+0.33,0.65,0.65);
    box.Draw(x+0.415,y+0.3);
    for(int i=0;i<5;i++) {
        general_text->text = descriptions[current_item][i];
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
        general_text->text = descriptions[current_item][i];
        general_text->Draw(x+0.415,y+0.30-0.05*i,1.3,1.3);
    }
    if(items.size() > display_num) {
        //draw the scroll bar
        //first the top arrow
        scrollbar->Draw(x,y,1,1);
    }
    pthread_mutex_unlock(&items_mutex);
}


