#pragma once

extern "C" {
void* ossia_ossia_new(t_symbol*, long, t_atom*);
void ossia_ossia_free(ossia::max::t_device*);
}
