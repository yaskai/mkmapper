#include <stdio.h>
#include <string.h>
#include "config.h"

void ConfRead(Config *conf, char *file_path) {
	FILE *pf = fopen(file_path, "r"); 

	if(!pf) {
		puts("ERROR: COULD NOT LOAD CONFIGURATION FILE");
		return;
	}

	puts("READING CONFIGURATION FILE...");

	char line[128];
	while(fgets(line, sizeof(line), pf)) {
		if(line[0] == COMMENT_MARKER) continue;
		
		if(line[0] == FLAG_MARKER) {
			// TODO: parse window flags
			continue;
		}
		
		ConfParseLine(conf, line);
	}

	printf("\nrefresh_rate=%d\n", conf->refresh_rate);
	printf("window_width=%d\n", conf->window_width);
	printf("window_height=%d\n\n", conf->window_height);
}

void ConfParseLine(Config *conf, char *line) {
	char *eq = strchr(line, '=');
	if(!eq) return;

	*eq = '\0';
	char *key = line;
	char *val = eq + 1;

	if(strcmp(key, "refresh_rate") == 0) 
		sscanf(val, "%d", &conf->refresh_rate);
	else if(strcmp(key, "window_width") == 0) 
		sscanf(val, "%d", &conf->window_width);
	else if(strcmp(key, "window_height") == 0) 
		sscanf(val, "%d", &conf->window_height);
	else if(strcmp(key, "gui_style") == 0)
		sscanf(val, "%d", &conf->gui_style);			
}

