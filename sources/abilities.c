#include "../includes/abilities.h"
#include "../includes/gameProperties.h"

typedef struct Enemyhealth
{
	int id;
	int health;
}Enemyhealth;


typedef struct Ability
{
	int unlocked;
	int cost;
	
}Ability;

typedef struct Abilities
{
	Ability psx;
	Ability kill;
}Abilities;

Abilities* get_abilities()
{
	static Abilities a;
	return &a;
}

void init_abilities()
{	
	Abilities * abl = get_abilities();
	abl->psx.unlocked = 1;
	abl->psx.cost = PSX_COST;
	abl->kill.unlocked = 0;
	abl->kill.cost = KILL_COST;
}
	
void unlock_ability(AbilityID id)
{
	char unlockstring[17] = "Ability unlocked";
	Abilities *abl = get_abilities();
	switch(id)
	{
		case PSX:
		{
			abl->psx.unlocked = 1;
			break;
		}
		case KILL:
		{
			abl->kill.unlocked = 1;
			useMemory(getGame(NULL), KILL_UNLOCK_COST);
			break;
		}
		default: 
		{
			fprintf(stderr, "Ability Switch Error\n");
			exit(1);
		}
	}
	textToTowerMonitor(unlockstring);		
}

int get_ability_cost(AbilityID id)
{
	Ability *a;
	switch(id)
	{
		case PSX:
		{
			a = &(get_abilities()->psx);
			break;
		}
		case KILL:
		{
			a = &(get_abilities()->kill);
			break;
		}
		default:
		{
			fprintf(stderr, "Is valid unlock Switch Error\n");
			exit(1);
		}
	}
	return a->cost;
}


int is_valid_unlock(AbilityID id)
{
	Ability *a;
	switch(id)
	{
		case PSX:
		{
			a = &(get_abilities()->psx);
			break;
		}
		case KILL:
		{
			a = &(get_abilities()->kill);
			break;
		}
		default:
		{
			fprintf(stderr, "Is valid unlock Switch Error\n");
			exit(1);
		}
	}
	if(a->unlocked == 0)
	{
		return 1;
	}
	return 0;
}
int is_available_ability(AbilityID id)
{

	Ability *a;
	switch(id)
	{
		case PSX:
		{
			a = &(get_abilities()->psx);
			break;
		}
		case KILL:
		{
			a = &(get_abilities()->kill);
			break;
		}
		default: 
		{
			fprintf(stderr, "Is avail Ability Switch Error\n");
			exit(1);
		}
	}
	if(a->unlocked == 1 && getTotalMemory() >= a->cost)
	{
		return 1;
	}
	return 0;
}

void apt_get_query()
{
	char *statuslist = (char*) calloc(200, sizeof(char));
	char template[100] = {"APT_GET_QUERY\n\n"};
	char psxlocked[50] = "PSX ability is locked\n";
	char killlocked[50] = "Kill ability is locked\n";
	char psxunlocked[50] = "PSX ability is unlocked\n";
	char killunlocked[50] = "Kill ability is unlocked\n";
	Abilities *a;	
	Ability *b, *c;

	a = get_abilities();
	b = &a->psx;
	c = &a->kill;
	
	strcpy(statuslist, template);
	if(b->unlocked == 1)
	{
		strcat(statuslist, psxunlocked);
	}
	else
	{
		strcat(statuslist, psxlocked);
	}
	if(c->unlocked == 1)
	{
		strcat(statuslist, killunlocked);
	}
	else
	{
		strcat(statuslist, killlocked);
	}

	sprintf(statuslist, "Kill ability unlock cost = %d", KILL_UNLOCK_COST);
	textToTowerMonitor(statuslist);
} 
	


int compare_health(const void*a, const void*b)
{
	const Enemyhealth* e1 = (const Enemyhealth*) a;
	const Enemyhealth* e2 = (const Enemyhealth*) b;

	return (e2->health - e1->health);
}

void psx_ability()
{
	char *psxlist = (char*) calloc(500,sizeof(char));	
	char line[32];
	int enemy_number = getNumberOfEnemies();
	int health = 0, ID = 0, i, j;
	printf("%d\n", enemy_number);


	Enemyhealth *e_health = (Enemyhealth*) calloc(enemy_number + 1, sizeof(Enemyhealth));
	{
		for(j = 1; j <= enemy_number; j++)
		{
			e_health[j].health = getEnemyHealth(j);
			e_health[j].id = j;
		}

		qsort(&e_health[1], enemy_number, sizeof(Enemyhealth), &compare_health);
		strcpy(psxlist, "EnemyID Health\n");
		for(i = 1; i <= enemy_number; i++)
		{
			if(!isDead(i))
			{
				ID = e_health[i].id;
				health = e_health[i].health;
				sprintf(line, "%d                %d\n", ID, health);
				strcat(psxlist, line);
			}
		}
        textToTowerMonitor(psxlist);
		test_psx_string(psxlist);
		printf("%s\n", psxlist);
		psxlist[0] = '\0';
	}
}

int kill_ability(int enemyID)
{
	if(is_available_ability(KILL) == 1)
	{
		{
			killEnemy(enemyID);
		}
		useMemory(getGame(NULL), KILL_COST);
		return 1;
	}
	return 0;
}

int kill_all_ability()
{
	int i;
	int enemy_number = getNumberOfEnemies();

	if(is_available_ability(KILL) == 1)
	{
		for(i = 1; i <= enemy_number; i++)
		{
			drawKillAll();
			killEnemy(i);
		}
		useMemory(getGame(NULL), KILL_ALL_COST);
		return 1;
	}
	return 0;
}

char *test_psx_string(char *psxlist)
{
	static char list[100];
	if(psxlist != NULL)
	{
		strcpy(list, psxlist);
	}
	return list;
}

void testAbilities()
{
	sput_start_testing();
	sput_set_output_stream(NULL);
	
	//sput_enter_suite("psx_ability():Testing Abilities - Info window");
	//sput_run_test(testAbilitiestoInfoWindow);
	//sput_leave_suite();

	sput_enter_suite("psx_ability(): Testing PSX");
	sput_run_test(testpsx);
	sput_leave_suite();
	
	sput_enter_suite("kill_all_ability(): Testing Kill all");
	sput_run_test(testkillall);
	sput_leave_suite();

	sput_finish_testing();
}


void testpsx()
{
	createEnemy();
	setEnemyHealth(1,100);
	int enemy_number = getNumberOfEnemies();
	sput_fail_if(enemy_number != 1, "Enemies found should = 1");
	psx_ability();
	sput_fail_if(strlen(test_psx_string(NULL)) == 0, "String not created");
	freeAllEnemies();
} 

void testkillall()
{
	createEnemy();
	setEnemyHealth(1,100);
	int enemy_number = getNumberOfEnemies();

	sput_fail_if(enemy_number != 1, "Enemies found should = 1");
	kill_all_ability();
	sput_fail_unless(getEnemyHealth(1) == 0, "Enemy should be killed");
	freeAllEnemies();
}


