/* ev-stack-switcher.h
 *
 * Copyright 2024 Christopher Davis <christopherdavis@gnome.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define EV_TYPE_STACK_SWITCHER (ev_stack_switcher_get_type())

G_DECLARE_FINAL_TYPE (EvStackSwitcher, ev_stack_switcher, EV, STACK_SWITCHER, AdwBin)

EvStackSwitcher *ev_stack_switcher_new (void);

void ev_stack_switcher_set_stack (EvStackSwitcher *self,
				  GtkStack        *stack);

G_END_DECLS
