
#include "g72x.h"
#include <string.h>
#include <stdio.h>

int
g721_encode_window(
	short* in,
	unsigned char* out,
	struct g72x_state *state_ptr)
{
    int i = 0;
    int chunk = 0;
    int bit_field = 0;
    struct G721_ENCODED_CHUNK* enc_chunk = (struct G721_ENCODED_CHUNK*)&bit_field;
    short* next_chunk_in = in;
    unsigned char* next_chunk_out = out;

    // clear the out buffer
    memset(out, 0, G721_BYTES_PER_EFRAME);
    
    for (chunk = 0; chunk < G721_CHUNKS; chunk++)
    {
        next_chunk_in = in + (chunk * G721_SAMPLES_PER_CHUNK);
        next_chunk_out = out + (chunk * G721_BYTES_PER_ECHUNK);

        // copy into a 32bit aligned variable. we have to do this because
        // thats the restriction of bit field structures        
        if (sizeof(bit_field) < G721_BYTES_PER_ECHUNK)
        {
            printf("ERROR: bit field must be less than bit structure!\n");
            return 0;
        }

        //printf("g724_24_encode: chunk(%d): next_chunk_in = 0x%p (%d), next_chunk_out = 0x%p (%d)\n",
        //       chunk,
        //       next_chunk_in, (next_chunk_in-in),
        //       next_chunk_out, (((unsigned char*)enc_chunk)-out));

        for (i = 0; i < G721_SAMPLES_PER_CHUNK; i++)
        {
            //printf("  sample %d\n", i);
            switch(i)
            {
            case 0:                
                enc_chunk->f0 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 1:
                enc_chunk->f1 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 2:
                enc_chunk->f2 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 3:
                enc_chunk->f3 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 4:
                enc_chunk->f4 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 5:
                enc_chunk->f5 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 6:
                enc_chunk->f6 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            case 7:
                enc_chunk->f7 = g721_encoder(next_chunk_in[i], AUDIO_ENCODING_LINEAR, state_ptr);
                break;
            default:
                break;
            }
        }

        // copy this into the output 
        memcpy(next_chunk_out, &bit_field,  G721_BYTES_PER_ECHUNK);
    }

    return (G721_BYTES_PER_EFRAME);
}


int
g721_decode_window(
	unsigned char* in,
	short* out,
	struct g72x_state *state_ptr)
{

    int chunk = 0;
    int bit_field;
    struct G721_ENCODED_CHUNK* enc_chunk = (struct G721_ENCODED_CHUNK*)&bit_field;
    unsigned char* next_chunk_in = in;
    short* next_chunk_out = out;

    // clear the out buffer
    memset(out, 0, G721_SAMPLES_PER_FRAME * sizeof(short));

    for (chunk = 0; chunk < G721_CHUNKS; chunk++)
    {
        next_chunk_in = in + (chunk * G721_BYTES_PER_ECHUNK);
        next_chunk_out = out + (chunk * G721_SAMPLES_PER_CHUNK);

        // copy into a 32bit aligned variable. we have to do this because
        // thats the restriction of bit field structures        
        if (sizeof(bit_field) < G721_BYTES_PER_ECHUNK)
        {
            printf("ERROR: bit field must be less than bit structure!\n");
            return 0;
        }
        
        // copy the in data into our bit_field
        memcpy(&bit_field, next_chunk_in, G721_BYTES_PER_ECHUNK);
        
        next_chunk_out[0] = g721_decoder(enc_chunk->f0, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[1] = g721_decoder(enc_chunk->f1, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[2] = g721_decoder(enc_chunk->f2, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[3] = g721_decoder(enc_chunk->f3, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[4] = g721_decoder(enc_chunk->f4, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[5] = g721_decoder(enc_chunk->f5, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[6] = g721_decoder(enc_chunk->f6, AUDIO_ENCODING_LINEAR, state_ptr);
        next_chunk_out[7] = g721_decoder(enc_chunk->f7, AUDIO_ENCODING_LINEAR, state_ptr);
    }

    return (G721_SAMPLES_PER_FRAME * sizeof(short));

}
