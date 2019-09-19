#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "stackctrl.h"
#include "sndmixer.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#define MODSNDMIXER_NUM_CHANNELS 2

mp_obj_t modsndmixer_callbacks[SNDMIXER_NUM_IDS];

static size_t modsndmixer_get_data_from_callable(void *_obj, size_t len, uint8_t *buffer) {
  mp_obj_t obj = _obj;
  mp_obj_type_t *type = mp_obj_get_type(obj);

  void* thread = mp_thread_get_state();
  printf("Thread state: %x\n", (size_t)thread);

  mp_obj_t arg[1];
  arg[0] = mp_obj_new_int_from_ull(len);
  printf("Arg at %x\n", (size_t)arg);
  mp_obj_t args = mp_obj_new_tuple(1, &arg[0]);
  printf("Args at %x\n", (size_t)args);

  printf("Calling %x, stack usage %d\n", (size_t)type->call, mp_stack_usage());
  mp_obj_t result = type->call(obj, 1, 0, &args);
  printf("Result at %x\n", (size_t)result);

  if(!MP_OBJ_IS_TYPE(result, &mp_type_bytes)) {
    return 0;
  }

  size_t out_len = 0;
  const char *data = mp_obj_str_get_data(result, &out_len);
  printf("Data at %x, len %d\n", (size_t)data, out_len);

  if(out_len > len) {
    out_len = len;
  }

  printf("Copying to %x\n", (size_t)buffer);
  memcpy(buffer, data, out_len);
  return out_len;
}

static mp_obj_t modsndmixer_start() {
  for(int i = 0; i < SNDMIXER_NUM_IDS; i++) {
    modsndmixer_callbacks[i] = mp_const_none;
  }
  printf("%d\n",mp_stack_usage() );
  sndmixer_init(MODSNDMIXER_NUM_CHANNELS);
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(modsndmixer_start_obj, modsndmixer_start);

static mp_obj_t modsndmixer_mod(mp_obj_t fun) {
  int id=sndmixer_queue_mod(&modsndmixer_get_data_from_callable, fun, 1);
  modsndmixer_callbacks[id-1] = fun;
  return mp_obj_new_int(id);
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_mod_obj, modsndmixer_mod);

static mp_obj_t modsndmixer_raw_s8(mp_obj_t fun) {
  int id=sndmixer_queue_raw_s8(&modsndmixer_get_data_from_callable, fun, 1);
  modsndmixer_callbacks[id-1] = fun;
  return mp_obj_new_int(id);
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_raw_s8_obj, modsndmixer_raw_s8);

static mp_obj_t modsndmixer_wav(mp_obj_t fun) {
  int id=sndmixer_queue_wav(&modsndmixer_get_data_from_callable, fun, 1);
  modsndmixer_callbacks[id-1] = fun;
  return mp_obj_new_int(id);
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_wav_obj, modsndmixer_wav);

static mp_obj_t modsndmixer_play(mp_obj_t _id) {
  int id = mp_obj_get_int(_id);
  sndmixer_play(id);
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_play_obj, modsndmixer_play);

static mp_obj_t modsndmixer_stop(mp_obj_t _id) {
  int id = mp_obj_get_int(_id);
  sndmixer_stop(id);
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_stop_obj, modsndmixer_stop);

static mp_obj_t modsndmixer_volume(mp_obj_t _id, mp_obj_t _volume) {
  int id = mp_obj_get_int(_id);
  int volume = mp_obj_get_int(_volume);
  sndmixer_set_volume(id, volume);
  return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(modsndmixer_volume_obj, modsndmixer_volume);

static const mp_rom_map_elem_t sndmixer_module_globals_table[] = {
  {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&modsndmixer_start_obj)},
  {MP_ROM_QSTR(MP_QSTR_raw_s8), MP_ROM_PTR(&modsndmixer_raw_s8_obj)},
  {MP_ROM_QSTR(MP_QSTR_wav), MP_ROM_PTR(&modsndmixer_wav_obj)},
  {MP_ROM_QSTR(MP_QSTR_mod), MP_ROM_PTR(&modsndmixer_mod_obj)},
  {MP_ROM_QSTR(MP_QSTR_volume), MP_ROM_PTR(&modsndmixer_volume_obj)},
  {MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&modsndmixer_start_obj)},
  {MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&modsndmixer_stop_obj)},
  {MP_ROM_QSTR(MP_QSTR__no_touchy_da_fishy), MP_ROM_PTR(&modsndmixer_callbacks)},
};

static MP_DEFINE_CONST_DICT(sndmixer_module_globals, sndmixer_module_globals_table);

const mp_obj_module_t sndmixer_module = {
  .base = {&mp_type_module},
  .globals = (mp_obj_dict_t *)&sndmixer_module_globals,
};

#endif
