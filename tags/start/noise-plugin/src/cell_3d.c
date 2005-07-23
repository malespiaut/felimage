/*  Felimage Noise Plugin for the GIMP
 *  Copyright (C) 2005 Guillermo Romero Franco <drirr_gato@users.sourceforge.net>
 *
 *  This file is part of the Felimage Noise Plugin for the GIMP
 *  
 *  Felimage Noise Plugin for the Gimp is free software; 
 *  you can redistribute it and/or modify it under the terms of 
 *  the GNU General Public License as published by the Free Software 
 *  Foundation; either version 2 of the License, or (at your option) 
 *  any later version.
 *
 *  Felimage Noise Plugin for the Gimp is distributed in the hope 
 *  that it will be useful, but WITHOUT ANY WARRANTY; without even 
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 *  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with fimg-noise; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
*/

/*
 *  This file has been adapted from the cellular texture basis function 
 *  source by Steven Worley, with the original copyight as follows:
 */
 
/* Copyright 1994, 2002 by Steven Worley
   This software may be modified and redistributed without restriction
   provided this comment header remains intact in the source code.
   This code is provided with no warrantee, express or implied, for
   any purpose.
   
   A detailed description and application examples can be found in the
   1996 SIGGRAPH paper "A Cellular Texture Basis Function" and
   especially in the 2002 book "Texturing and Modeling, a Procedural
   Approach, 3rd edition." There is also extra information on the web
   site http://www.worley.com/cellular.html .

   If you do find interesting uses for this tool, and especially if
   you enhance it, please drop me an email at steve@worley.com. */


#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


#ifdef CALIBRATE
#include <glib.h>
#include <string.h>
#else
#include <libgimp/gimp.h>
#endif

#include "poisson.h"
#include "cell_int.h" 
#include "cell.h"
#include "random.h"

#define DENSITY_ADJUSTMENT  1.0
/*0.398150*/

static void AddSamples_3D(gint32 xi, gint32 yi, gint32 zi, 
		gint32 max_order, double at[3], double *F, 
		double (*delta)[3], guint32 *ID, 
		int order[], CellBasisCache3D *cache );

CellBasisCache3D *InitCellBasis3D() {
	CellBasisCache3D *cache;

	cache = calloc(1,sizeof(CellBasisCache3D) * (3*3*3)); 

	return cache;
}

void FinishCellBasis3D(CellBasisCache3D *cache) {
	free(cache);
}

void Cells3D(double a0, double a1, double a2, gint32 max_order, double *f, double (*p_delta)[3], guint32 *p_id, CellBasisCache3D *cache) {
	double pa0,pa1,pa2, ma0,ma1,ma2;
	double new_at[3];
	gint32 i,j;
	gint32 int_at[3];
	gint32 int_at_p[3];
	gint32 int_at_m[3];
	double *f_max;
	double delta[4][3];
	guint32 id[4];

	int order[]={0,1,2,3,4};

	for (i=0; i<max_order; i++) f[i]=999999.9;

	new_at[0]=DENSITY_ADJUSTMENT*a0;
	new_at[1]=DENSITY_ADJUSTMENT*a1;
	new_at[2]=DENSITY_ADJUSTMENT*a2;

	int_at[0]=(new_at[0]<0.0) ? (gint32)new_at[0]-1 : (gint32)new_at[0]; 
	int_at[1]=(new_at[1]<0.0) ? (gint32)new_at[1]-1 : (gint32)new_at[1]; 
	int_at[2]=(new_at[2]<0.0) ? (gint32)new_at[2]-1 : (gint32)new_at[2]; 

	ma0=new_at[0]-int_at[0];
	ma1=new_at[1]-int_at[1];
	ma2=new_at[2]-int_at[2];
	pa0=(1.0-ma0)*(1.0-ma0);
	pa1=(1.0-ma1)*(1.0-ma1);
	pa2=(1.0-ma2)*(1.0-ma2);
	ma0*=ma0;
	ma1*=ma1;
	ma2*=ma2;

	int_at_p[0] = int_at[0]+1;
	int_at_p[1] = int_at[1]+1;
	int_at_p[2] = int_at[2]+1;

	int_at_m[0] = int_at[0]-1;
	int_at_m[1] = int_at[1]-1;
	int_at_m[2] = int_at[2]-1;

	f_max = f+(max_order-1);

	/* as generated by gen_tests.py */
  
        AddSamples_3D( int_at[0], int_at[1], int_at[2], max_order, new_at, f, delta, id, order, cache);

        if (pa0 < *f_max) AddSamples_3D( int_at_p[0], int_at[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 1 );
        if (pa1 < *f_max) AddSamples_3D( int_at[0], int_at_p[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 2 );
        if (pa2 < *f_max) AddSamples_3D( int_at[0], int_at[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 3 );
        if (ma2 < *f_max) AddSamples_3D( int_at[0], int_at[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 4 );
        if (ma1 < *f_max) AddSamples_3D( int_at[0], int_at_m[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 5 );
        if (ma0 < *f_max) AddSamples_3D( int_at_m[0], int_at[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 6 );
        if (pa0 + pa1 < *f_max) AddSamples_3D( int_at_p[0], int_at_p[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 7 );
        if (pa0 + pa2 < *f_max) AddSamples_3D( int_at_p[0], int_at[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 8 );
        if (pa0 + ma2 < *f_max) AddSamples_3D( int_at_p[0], int_at[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 9 );
        if (pa0 + ma1 < *f_max) AddSamples_3D( int_at_p[0], int_at_m[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 10 );
        if (pa1 + pa2 < *f_max) AddSamples_3D( int_at[0], int_at_p[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 11 );
        if (pa1 + ma2 < *f_max) AddSamples_3D( int_at[0], int_at_p[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 12 );
        if (ma1 + pa2 < *f_max) AddSamples_3D( int_at[0], int_at_m[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 13 );
        if (ma1 + ma2 < *f_max) AddSamples_3D( int_at[0], int_at_m[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 14 );
        if (ma0 + pa1 < *f_max) AddSamples_3D( int_at_m[0], int_at_p[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 15 );
        if (ma0 + pa2 < *f_max) AddSamples_3D( int_at_m[0], int_at[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 16 );
        if (ma0 + ma2 < *f_max) AddSamples_3D( int_at_m[0], int_at[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 17 );
        if (ma0 + ma1 < *f_max) AddSamples_3D( int_at_m[0], int_at_m[1], int_at[2], max_order, new_at, f, delta, id, order, cache + 18 );
        if (pa0 + pa1 + pa2 < *f_max) AddSamples_3D( int_at_p[0], int_at_p[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 19 );
        if (pa0 + pa1 + ma2 < *f_max) AddSamples_3D( int_at_p[0], int_at_p[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 20 );
        if (pa0 + ma1 + pa2 < *f_max) AddSamples_3D( int_at_p[0], int_at_m[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 21 );
        if (pa0 + ma1 + ma2 < *f_max) AddSamples_3D( int_at_p[0], int_at_m[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 22 );
        if (ma0 + pa1 + pa2 < *f_max) AddSamples_3D( int_at_m[0], int_at_p[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 23 );
        if (ma0 + pa1 + ma2 < *f_max) AddSamples_3D( int_at_m[0], int_at_p[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 24 );
        if (ma0 + ma1 + pa2 < *f_max) AddSamples_3D( int_at_m[0], int_at_m[1], int_at_p[2], max_order, new_at, f, delta, id, order, cache + 25 );
        if (ma0 + ma1 + ma2 < *f_max) AddSamples_3D( int_at_m[0], int_at_m[1], int_at_m[2], max_order, new_at, f, delta, id, order, cache + 26 );


	for (i=0; i<max_order; i++) {
		f[i]=sqrt(f[i])*(1.0/DENSITY_ADJUSTMENT);      
		j = order[i];
		p_delta[i][0]=delta[j][0]*(1.0/DENSITY_ADJUSTMENT);
		p_delta[i][1]=delta[j][1]*(1.0/DENSITY_ADJUSTMENT);
		p_delta[i][2]=delta[j][2]*(1.0/DENSITY_ADJUSTMENT);
		p_id[i] = id[j];
	}

	return;
}


static void AddSamples_3D(gint32 xi, gint32 yi, gint32 zi, 
		gint32 max_order, double at[3], double *F, 
		double (*delta)[3], guint32 *ID, 
		int order[], CellBasisCache3D *cache ) {

	double dx, dy, dz, fx, fy, fz, d2;
	gint32 count, i, j, index;
	int slot;
	guint32 seed, this_id;

	seed = Hash3(xi,yi,zi);
	count=Poisson_count[seed&255];

	if ( (seed != cache->last_seed) || (seed == 0)) {
		cache->last_seed = seed;	

		seed=RANDOM(seed);
		for (j=0; j<count; j++) {
			cache->id[j] = this_id = seed;
			
			seed=RANDOM(seed);
			fx = (seed+0.5)*(1.0/4294967296.0); 
			
			seed=RANDOM(seed);
			fy = (seed+0.5)*(1.0/4294967296.0);
			
			seed=RANDOM(seed);
			fz =(seed+0.5)*(1.0/4294967296.0);

			seed=RANDOM(seed);

			dx = (cache->p[0][j] = fx)+xi-at[0]; 
			dy = (cache->p[1][j] = fy)+yi-at[1];
			dz = (cache->p[2][j] = fz)+zi-at[2];

			d2 = dx*dx+dy*dy+dz*dz;

			i = max_order-1;
			
			if (d2<F[i]) {
				
				index=max_order;
				while (index>0 && d2<F[index-1]) index--;

				slot = order[i];

				while ((i--)>index) {
					F[i+1]=F[i];
					order[i+1] = order[i];
				}		
				
				F[index]=d2;
				order[index] = slot;
					
				ID   [slot]=this_id;
				delta[slot][0]=dx;
				delta[slot][1]=dy;
				delta[slot][2]=dz;
			}
		}
	} else {
		for (j=0; j<count; j++) {
			dx = cache->p[0][j] + xi - at[0];
			dy = cache->p[1][j] + yi - at[1];
			dz = cache->p[2][j] + zi - at[2];
			
			d2 = dx*dx+dy*dy+dz*dz;

			i = max_order-1;
			
			if (d2<F[i]) {
				index=max_order;
				while (index>0 && d2<F[index-1]) index--;

				slot = order[i];

				while ((i--)>index) {
					F[i+1]=F[i];
					order[i+1] = order[i];
				}		
				
				F[index]=d2;
				order[index] = slot;
					
				ID   [slot]= cache->id[j];
				delta[slot][0]=dx;
				delta[slot][1]=dy;
				delta[slot][2]=dz;
			}
		}
	}

	return;
}
