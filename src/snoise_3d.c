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

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CALIBRATE
#include <glib.h>
#include <string.h>
#else
#include <libgimp/gimp.h>
#endif

#include "poisson.h"
#include "random.h"
#include "snoise.h"
#include "snoise_int.h"

SNoiseBasisCache3D*
InitSNoiseBasis3D()
{
  SNoiseBasisCache3D* cache;

  cache = g_malloc(sizeof(SNoiseBasisCache3D) * (3 * 3 * 3));
  memset(cache, 0, sizeof(SNoiseBasisCache3D) * (3 * 3 * 3));

  return cache;
}

void
FinishSNoiseBasis3D(SNoiseBasisCache3D* cache)
{
  g_free(cache);
}

double
SNoise3D(double a0, double a1, double a2, SNoiseBasisCache3D* cache)
{
  int a[3];
  guint32 seed[3];
  int count, j;
  double d[3];
  double f[3];
  gint32 int_at[3];
  double dist;
  double r;
  double fa[3];

  int_at[0] = (a0 < 0.0) ? (gint32)a0 - 1 : (gint32)a0;
  int_at[1] = (a1 < 0.0) ? (gint32)a1 - 1 : (gint32)a1;
  int_at[2] = (a2 < 0.0) ? (gint32)a2 - 1 : (gint32)a2;

  r = 0;

  for (a[2] = -1; a[2] <= 1; a[2]++)
  {
    seed[2] = Hash1(a[2] + int_at[2]);
    fa[2] = a[2] + int_at[2] - a2;
    for (a[1] = -1; a[1] <= 1; a[1]++)
    {
      seed[1] = Hash1(seed[2] + a[1] + int_at[1]);
      fa[1] = a[1] + int_at[1] - a1;
      for (a[0] = -1; a[0] <= 1; a[0]++)
      {
        seed[0] = Hash1(seed[1] + a[0] + int_at[0]);
        fa[0] = a[0] + int_at[0] - a0;

        count = Poisson_count[seed[0] & 255];

        if ((seed[0] != cache->last_seed) || (seed[0] == 0))
        {
          cache->last_seed = seed[0];

          for (j = 0; j < count; j++)
          {
            seed[0] = RANDOM(seed[0]);
            f[0] = (seed[0] + 0.5) * (1.0 / 4294967296.0);

            seed[0] = RANDOM(seed[0]);
            f[1] = (seed[0] + 0.5) * (1.0 / 4294967296.0);

            seed[0] = RANDOM(seed[0]);
            f[2] = (seed[0] + 0.5) * (1.0 / 4294967296.0);

            d[0] = (cache->p[0][j] = f[0]) + fa[0];
            d[1] = (cache->p[1][j] = f[1]) + fa[1];
            d[2] = (cache->p[2][j] = f[2]) + fa[2];

            dist = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];

            if (dist < 1.00)
            {
              r += KERNEL(dist);
            }
          }
        }
        else
        {
          for (j = 0; j < count; j++)
          {
            d[0] = cache->p[0][j] + fa[0];
            d[1] = cache->p[1][j] + fa[1];
            d[2] = cache->p[2][j] + fa[2];

            dist = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];

            if (dist < 1.00)
            {
              r += KERNEL(dist);
            }
          }
        }

        cache++;
      }
    }
  }

  return r;
}
