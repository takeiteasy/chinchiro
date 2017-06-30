//
//  game_obj.c
//  cee-lo
//
//  Created by George Watson on 29/06/2017.
//  Copyright © 2017 George Watson. All rights reserved.
//

#include "game_obj.h"

static mat4 dice_scale = {
	.1f, 0.f, 0.f, 0.f,
	0.f, .1f, 0.f, 0.f,
	0.f, 0.f, .1f, 0.f,
	0.f, 0.f, 0.f, 1.f
};
static const dReal *t, *r;

void draw_game_obj(game_obj_t* o, GLuint model_loc, GLuint texture_loc) {
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, &o->world.m[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, o->texture);
	glUniform1i(texture_loc, 0);

	draw_obj(o->model);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void update_game_obj(game_obj_t* o, int scale) {
	t = dBodyGetPosition(o->body);
	r = dBodyGetRotation(o->body);

	o->world = mat4_new(r[0], r[1], r[2],  t[0],
			r[4], r[5], r[6],  t[1],
			r[8], r[9], r[10], t[2],
			0.f,  0.f,  0.f,   1.f);
	if (scale)
		o->world = mat4_mul_mat4(o->world, dice_scale);
}

void free_game_obj(game_obj_t* o) {
	if (o->geom)
		dGeomDestroy(o->geom);
	if (o->model) {
		free_obj(o->model);
		o->model = NULL;
	}
	if (o->texture)
		glDeleteTextures(1, &o->texture);
}
