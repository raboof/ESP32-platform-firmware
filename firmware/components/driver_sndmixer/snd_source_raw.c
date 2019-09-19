#include <sdkconfig.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sndmixer.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#define CHUNK_SIZE 32

enum {
  RF_U8, RF_U16, RF_S8, RF_S16
};

typedef struct {
  snd_src_input_data_callback callback;
  void *callback_obj;
  int rate;
  int format;
} raw_ctx_t;

int raw_init_source(const snd_src_input_data_callback callback, void *callback_obj, int sample_rate, void **ctx, int format)
{
  raw_ctx_t *raw=calloc(sizeof(raw_ctx_t), 1);
  if (!raw) goto err;

  raw->callback = callback;
  raw->callback_obj = callback_obj;
  raw->rate = sample_rate;
  raw->format = RF_S8;
  *ctx=(void*)raw;
  return CHUNK_SIZE;

 err:
  free(raw);
  return -1;
}

int raw_init_source_s8(snd_src_input_data_callback callback, void *callback_obj, int req_sample_rate, void **ctx) {
  return raw_init_source(callback, callback_obj, req_sample_rate, ctx, RF_S8);
}

int raw_get_sample_rate(void *ctx) {
  raw_ctx_t *raw=(raw_ctx_t*)ctx;
  return raw->rate;
}

size_t raw_fill_buffer_from_s8(raw_ctx_t *raw, int8_t *buffer, size_t len) {
  printf("Calling %x with %x and len %d\n", (size_t)raw->callback, (size_t)raw->callback_obj, len);
  return raw->callback(raw->callback_obj, len, (uint8_t*)buffer);
}

size_t raw_fill_buffer(void *ctx, int8_t *buffer) {
  printf("Filling buffer for %x -> %x\n", (size_t)ctx, (size_t)buffer);
  raw_ctx_t *raw=(raw_ctx_t*)ctx;
  size_t ret = 0;
  switch(raw->format) {
  case RF_S8:
    ret = raw_fill_buffer_from_s8(raw, buffer, CHUNK_SIZE);
    break;
  }
  return ret;
}

void raw_deinit_source(void *ctx) {
  free(ctx);
}

const sndmixer_source_t sndmixer_source_raw_s8={
                                                .init_source=raw_init_source_s8,
                                                .get_sample_rate=raw_get_sample_rate,
                                                .fill_buffer=raw_fill_buffer,
                                                .deinit_source=raw_deinit_source
};

#endif
