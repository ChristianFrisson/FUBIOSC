#include "m_pd.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define NB_PARAM 7

static t_class *weightedinterpolation_class;

typedef struct _preset {
	float x;
	float y;
	float mass;
	float parameters[NB_PARAM];
}t_preset;

typedef struct _weightedinterpolation {
  t_object  x_obj;
  t_outlet *flist_out, *b_out;

  t_preset *presets;
  t_int presetsSize;
  t_float currentParameters[NB_PARAM];
  t_float power, dmin;
  int loaded;
} t_weightedinterpolation;

void displayparameters(t_weightedinterpolation *x)
{
	int i, j;
	for(i=0; i<x->presetsSize; i++)
	{
		post("x: %f, y: %f, mass: %f", (x->presets+i)->x, (x->presets+i)->y, (x->presets+i)->mass);
		post("parameters: ");
		for(j=0; j<NB_PARAM; j++)
		  post("parameter[%d]: %f ", j, (x->presets+i)->parameters[j]);
	}
}

int nextNumber(FILE* fp)
{
    char c = 0;
    while(c<'0' || c>'9')
    {
        c = fgetc(fp);
        if(c == EOF)
            return -1;
        if(c == '\n')
            return 0;
    }
    fseek(fp, -1, SEEK_CUR);
    return 1;
}

void weightedinterpolation_bang(t_weightedinterpolation *x)
{
  t_int i;
  t_atom cursorParameters[NB_PARAM];
  for(i=0; i<NB_PARAM; i++)
	  SETFLOAT(cursorParameters+i, (x->currentParameters)[i]);

  outlet_list(x->flist_out, &s_list, NB_PARAM, cursorParameters);
}

void weightedinterpolation_read(t_weightedinterpolation *x, t_symbol *s)
{
    FILE* fp;
	t_float number;
    int index = 0;
    int structindex = 0;

	fp = fopen(s->s_name, "r");
    if(fp == NULL)
	{
		error("Impossible to open the file \"%s\"", s->s_name);
		x->loaded = 0;
		return;
	}
	if(x->loaded)
		free(x->presets);

    while(nextNumber(fp)!=-1)
    {
        if(structindex == 0)
			x->presets = (t_preset*) malloc(sizeof(t_preset));
		else
			x->presets = (t_preset*) realloc(x->presets, (structindex+1)*sizeof(t_preset));
		
		index = 0;
        while(nextNumber(fp)==1)
        {
            fscanf(fp, "%f", &number);

            switch(index)
            {
                case 0:
                    //post("x = %f\n", number);
					(x->presets+structindex)->x = number;
                    break;
                case 1:
					 //post("y = %f\n", number);
                    (x->presets+structindex)->y = number;
                    break;
                case 2:
					//post("mass = %f\n", number);
                    (x->presets+structindex)->mass = number;
                    break;
                default:
                    (x->presets+structindex)->parameters[index-3] = number;
                    break;
            }
			index++;
        }
        structindex++;
    }
    x->presetsSize = structindex;
    fclose(fp);
	x->loaded = 1;
	
	post("file loaded: \"%s\"", s->s_name);
	post("Number of presets: %d", x->presetsSize);
	//displayparameters(x);
}

void weightedinterpolation_cursorxy(t_weightedinterpolation *x, t_floatarg curx, t_floatarg cury)
{
  t_int i, j;
  t_float totalWeight;
  t_float *temp; 
  t_float *weights;
  t_float distance;
  t_float prex, prey;
  t_atom cursorParameters[NB_PARAM];
  
  //calculating weights (function of distance)
  distance = 0;
  weights = (t_float*) malloc(x->presetsSize*sizeof(t_float));
  for(i=0; i<x->presetsSize; i++)
  {
	  prex = (x->presets+i)->x;
	  prey = (x->presets+i)->y;
	  distance = sqrt((curx-prex)*(curx-prex)+(cury-prey)*(cury-prey));
	  if(distance < x->dmin)
		  distance = x->dmin;
	  weights[i] = (x->presets+i)->mass/pow(distance, x->power);
  }
  
  //normalizing weights
  totalWeight = 0;
  for(i=0; i<x->presetsSize; i++)
	totalWeight+=weights[i];
  for(i=0; i<x->presetsSize; i++)
	weights[i]/=totalWeight;
  
  //multiplying each parameter by its weight
  temp = (t_float*) malloc(NB_PARAM*sizeof(t_float));
  for(j=0; j<NB_PARAM; j++)
	  temp[j] = 0;
  for(i=0; i<x->presetsSize; i++)
  {
	  for(j=0; j<NB_PARAM; j++)
		  temp[j]+=((x->presets+i)->parameters[j]*weights[i]);
  }
  
  //sending values to the outlet
  for(j=0; j<NB_PARAM; j++)
  {
		SETFLOAT(cursorParameters+j, temp[j]);
		x->currentParameters[j] = temp[j];
  }
  outlet_list(x->flist_out, &s_list, NB_PARAM, cursorParameters);

  //free memory
  free(weights);
  free(temp);
}

void weightedinterpolation_getPreset(t_weightedinterpolation *x, t_floatarg index)
{
  t_int i, tempIndex;
  t_float temp[4]; 
  t_atom presetPosition[4];

  //getting values  
  tempIndex = (t_int)index;
  if(tempIndex < x->presetsSize && tempIndex >= 0)
  {
	temp[0] = tempIndex;
	temp[1] = x->presets[tempIndex].x;
	temp[2] = x->presets[tempIndex].y;
	temp[3] = x->presets[tempIndex].mass;
  }
  else
  {
	temp[0] = -1;
	temp[1] = 0;
	temp[2] = 0;
	temp[3] = 0;
  }

  //sending values to the outlet
  for(i=0; i<4; i++)
  {
		SETFLOAT(presetPosition+i, temp[i]);
  }
  outlet_list(x->flist_out, &s_list, 4, presetPosition);
}

void weightedinterpolation_power(t_weightedinterpolation *x, t_floatarg pow)
{
	x->power = pow;
	post("new power: %f", x->power);
}

void weightedinterpolation_dmin(t_weightedinterpolation *x, t_floatarg dm)
{
	x->dmin = dm;
	post("new dmin: %f", x->dmin);
}

void *weightedinterpolation_new(t_symbol *s, int argc, t_atom *argv)
{
  t_symbol *ms;
  t_weightedinterpolation *x = (t_weightedinterpolation *)pd_new(weightedinterpolation_class);
  x->dmin = 0.01;
  x->power = 2;
  x->loaded = 0;
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("bound"));

  x->flist_out = outlet_new(&x->x_obj, &s_list);
  x->b_out = outlet_new(&x->x_obj, &s_bang);

  switch(argc)
  {
	  case 1:
		  weightedinterpolation_read(x, atom_getsymbol(argv));
		  break;
	  default:
		  break;
  }

  return (void *)x;
}

static void weightedinterpolation_free(t_weightedinterpolation *x)
{
	if(x->loaded)
		free(x->presets);
}

void weightedinterpolation_setup(void) {
  weightedinterpolation_class = class_new(gensym("weightedinterpolation"), 
											(t_newmethod)weightedinterpolation_new, 
											(t_method)weightedinterpolation_free, 
											sizeof(t_weightedinterpolation), 
											CLASS_DEFAULT, A_GIMME, 0);

  class_addbang(weightedinterpolation_class, weightedinterpolation_bang);
  class_addmethod(weightedinterpolation_class, (t_method)weightedinterpolation_read, gensym("read"), A_DEFSYM, 0);
  class_addmethod(weightedinterpolation_class, (t_method)weightedinterpolation_cursorxy, gensym("cursor"), A_DEFFLOAT, A_DEFFLOAT, 0);
  class_addmethod(weightedinterpolation_class, (t_method)weightedinterpolation_getPreset, gensym("getPreset"), A_DEFFLOAT, 0);
  class_addmethod(weightedinterpolation_class, (t_method)weightedinterpolation_power, gensym("power"), A_DEFFLOAT, 0);
  class_addmethod(weightedinterpolation_class, (t_method)weightedinterpolation_dmin, gensym("dmin"), A_DEFFLOAT, 0);

  class_sethelpsymbol(weightedinterpolation_class, gensym("help-weightedinterpolation"));
}
