/* Aravis - Digital camera library
 *
 * Copyright © 2009-2010 Emmanuel Pacaud
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Emmanuel Pacaud <emmanuel@gnome.org>
 */

#include <arvgcswissknife.h>
#include <arvgcport.h>
#include <string.h>

static GObjectClass *parent_class = NULL;

/* ArvGcNode implementation */

typedef struct {
	char *name;
	char *node_name;
} ArvGcSwissKnifeVariableInfos;

static void
arv_gc_swiss_knife_add_element (ArvGcNode *node, const char *name, const char *content, const char **attributes)
{
	ArvGcSwissKnife *gc_swiss_knife = ARV_GC_SWISS_KNIFE (node);

	if (strcmp (name, "pVariable") == 0) {
		const char *variable_name = NULL;
		int i;

		for (i = 0; attributes[i] != NULL; i = i+2)
			if (g_strcmp0 (attributes[i], "Name")) {
				variable_name = attributes[i+1];
				break;
			}

		if (variable_name != NULL) {
			ArvGcSwissKnifeVariableInfos *variable_infos;

			variable_infos = g_new (ArvGcSwissKnifeVariableInfos, 1);
			variable_infos->name = g_strdup (variable_name);
			variable_infos->node_name = g_strdup (content);
			gc_swiss_knife->variables = g_slist_prepend (gc_swiss_knife->variables,
								     variable_infos);
		}
	} else if (strcmp (name, "Formula") == 0) {
		arv_evaluator_set_expression (gc_swiss_knife->formula, content);
	} else if (strcmp (name, "Expression") == 0) {
		g_assert_not_reached ();
	} else if (strcmp (name, "Constant") == 0) {
		g_assert_not_reached ();
	} else
		ARV_GC_NODE_CLASS (parent_class)->add_element (node, name, content, attributes);
}

/* ArvGcSwissKnife implementation */

ArvGcNode *
arv_gc_swiss_knife_new (void)
{
	ArvGcSwissKnife *swiss_knife;

	swiss_knife = g_object_new (ARV_TYPE_GC_SWISS_KNIFE, NULL);
	swiss_knife->is_integer = FALSE;

	return ARV_GC_NODE (swiss_knife);
}

ArvGcNode *
arv_gc_int_swiss_knife_new (void)
{
	ArvGcSwissKnife *swiss_knife;

	swiss_knife = g_object_new (ARV_TYPE_GC_SWISS_KNIFE, NULL);
	swiss_knife->is_integer = TRUE;

	return ARV_GC_NODE (swiss_knife);
}

static void
arv_gc_swiss_knife_init (ArvGcSwissKnife *gc_swiss_knife)
{
	gc_swiss_knife->formula = arv_evaluator_new ("");
}

static void
arv_gc_swiss_knife_finalize (GObject *object)
{
	ArvGcSwissKnife *gc_swiss_knife = ARV_GC_SWISS_KNIFE (object);
	GSList *iter;

	for (iter = gc_swiss_knife->variables; iter != NULL; iter = iter->next) {
		ArvGcSwissKnifeVariableInfos *variable_infos = iter->data;

		g_free (variable_infos->name);
		g_free (variable_infos->node_name);
		g_free (variable_infos);
	}
	g_slist_free (gc_swiss_knife->variables);

	g_object_unref (gc_swiss_knife->formula);

	parent_class->finalize (object);
}

static void
arv_gc_swiss_knife_class_init (ArvGcSwissKnifeClass *swiss_knife_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (swiss_knife_class);
	ArvGcNodeClass *node_class = ARV_GC_NODE_CLASS (swiss_knife_class);

	parent_class = g_type_class_peek_parent (swiss_knife_class);

	object_class->finalize = arv_gc_swiss_knife_finalize;

	node_class->add_element = arv_gc_swiss_knife_add_element;
}

/* ArvGcInteger interface implementation */

guint64
arv_gc_swiss_knife_get_integer_value (ArvGcInteger *gc_integer)
{
	return 0;
}

void
arv_gc_swiss_knife_set_integer_value (ArvGcInteger *gc_integer, guint64 value)
{
}

static void
arv_gc_swiss_knife_integer_interface_init (ArvGcIntegerInterface *interface)
{
	interface->get_value = arv_gc_swiss_knife_get_integer_value;
	interface->set_value = arv_gc_swiss_knife_set_integer_value;
}

G_DEFINE_TYPE_WITH_CODE (ArvGcSwissKnife, arv_gc_swiss_knife, ARV_TYPE_GC_NODE,
			 G_IMPLEMENT_INTERFACE (ARV_TYPE_GC_INTEGER, arv_gc_swiss_knife_integer_interface_init))
