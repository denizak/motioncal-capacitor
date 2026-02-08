#include "imuread.h"

void print_data(const char *name, const unsigned char *data, int len)
{
	int i;

	printf("%s (%2d bytes):", name, len);
	for (i = 0; i < len; i++)
	{
		printf(" %02X", data[i]);
	}
	printf("\n");
}

static int packet_primary_data(const unsigned char *data)
{
	current_orientation.q0 = (float)((int16_t)((data[25] << 8) | data[24])) / 30000.0f;
	current_orientation.q1 = (float)((int16_t)((data[27] << 8) | data[26])) / 30000.0f;
	current_orientation.q2 = (float)((int16_t)((data[29] << 8) | data[28])) / 30000.0f;
	current_orientation.q3 = (float)((int16_t)((data[31] << 8) | data[30])) / 30000.0f;

	return 1;
}

static int packet_magnetic_cal(const unsigned char *data)
{
	int16_t id, x, y, z;
	int n;

	id = (data[7] << 8) | data[6];
	x = (data[9] << 8) | data[8];
	y = (data[11] << 8) | data[10];
	z = (data[13] << 8) | data[12];

	if (id == 1)
	{
		magcal.V[0] = (float)x * 0.1f;
		magcal.V[1] = (float)y * 0.1f;
		magcal.V[2] = (float)z * 0.1f;
		return 1;
	}
	else if (id == 2)
	{
		magcal.invW[0][0] = (float)x * 0.001f;
		magcal.invW[1][1] = (float)y * 0.001f;
		magcal.invW[2][2] = (float)z * 0.001f;
		return 1;
	}
	else if (id == 3)
	{
		magcal.invW[0][1] = (float)x / 1000.0f;
		magcal.invW[1][0] = (float)x / 1000.0f; // TODO: check this assignment
		magcal.invW[0][2] = (float)y / 1000.0f;
		magcal.invW[1][2] = (float)y / 1000.0f; // TODO: check this assignment
		magcal.invW[1][2] = (float)z / 1000.0f;
		magcal.invW[2][1] = (float)z / 1000.0f; // TODO: check this assignment
		return 1;
	}
	else if (id >= 10 && id < MAGBUFFSIZE + 10)
	{
		n = id - 10;
		if (magcal.valid[n] == 0 || x != magcal.BpFast[0][n] || y != magcal.BpFast[1][n] || z != magcal.BpFast[2][n])
		{
			magcal.BpFast[0][n] = x;
			magcal.BpFast[1][n] = y;
			magcal.BpFast[2][n] = z;
			magcal.valid[n] = 1;
		}
		return 1;
	}
	return 0;
}

static int packet(const unsigned char *data, int len)
{
	if (len <= 0)
		return 0;

	if (data[0] == 1 && len == 34)
	{
		return packet_primary_data(data);
	}
	else if (data[0] == 6 && len == 14)
	{
		return packet_magnetic_cal(data);
	}
	return 0;
}

static int packet_encoded(const unsigned char *data, int len)
{
	const unsigned char *p;
	unsigned char buf[256];
	int buflen = 0, copylen;

	p = memchr(data, 0x7D, len);
	if (p == NULL)
	{
		return packet(data, len);
	}
	else
	{
		while (1)
		{
			copylen = p - data;
			if (copylen > 0)
			{
				if (buflen + copylen > sizeof(buf))
					return 0;
				memcpy(buf + buflen, data, copylen);
				buflen += copylen;
				data += copylen;
				len -= copylen;
			}
			if (buflen + 1 > sizeof(buf))
				return 0;
			buf[buflen++] = (p[1] == 0x5E) ? 0x7E : 0x7D;
			data += 2;
			len -= 2;
			if (len <= 0)
				break;
			p = memchr(data, 0x7D, len);
			if (p == NULL)
			{
				if (buflen + len > sizeof(buf))
					return 0;
				memcpy(buf + buflen, data, len);
				buflen += len;
				break;
			}
		}
		return packet(buf, buflen);
	}
}

static int packet_parse(const unsigned char *data, int len)
{
	static unsigned char packetbuf[256];
	static unsigned int packetlen = 0;
	const unsigned char *p;
	int copylen;
	int ret = 0;

	// print_data("packet_parse", data, len);
	while (len > 0)
	{
		p = memchr(data, 0x7E, len);
		if (p == NULL)
		{
			if (packetlen + len > sizeof(packetbuf))
			{
				packetlen = 0;
				return 0; // would overflow buffer
			}
			memcpy(packetbuf + packetlen, data, len);
			packetlen += len;
			len = 0;
		}
		else if (p > data)
		{
			copylen = p - data;
			if (packetlen + copylen > sizeof(packetbuf))
			{
				packetlen = 0;
				return 0; // would overflow buffer
			}
			memcpy(packetbuf + packetlen, data, copylen);
			packet_encoded(packetbuf, packetlen + copylen);
			packetlen = 0;
			data += copylen + 1;
			len -= copylen + 1;
		}
		else
		{
			if (packetlen > 0)
			{
				if (packet_encoded(packetbuf, packetlen))
					ret = 1;
				packetlen = 0;
			}
			data++;
			len--;
		}
	}
	return ret;
}

#define ASCII_STATE_WORD 0
#define ASCII_STATE_RAW 1
#define ASCII_STATE_CAL1 2
#define ASCII_STATE_CAL2 3

static int ascii_parse(const unsigned char *data, int len)
{
	static int ascii_state = ASCII_STATE_WORD;
	static int ascii_num = 0, ascii_neg = 0, ascii_count = 0;
	static int16_t ascii_raw_data[9];
	static float ascii_cal_data[10];
	static unsigned int ascii_raw_data_count = 0;
	const char *p, *end;
	int ret = 0;

	// print_data("ascii_parse", data, len);
	end = (const char *)(data + len);
	for (p = (const char *)data; p < end; p++)
	{
		if (ascii_state == ASCII_STATE_WORD)
		{
			if (ascii_count == 0)
			{
				if (*p == 'R')
				{
					ascii_num = ASCII_STATE_RAW;
					ascii_count = 1;
				}
				else if (*p == 'C')
				{
					ascii_num = ASCII_STATE_CAL1;
					ascii_count = 1;
				}
			}
			else if (ascii_count == 1)
			{
				if (*p == 'a')
				{
					ascii_count = 2;
				}
				else
				{
					ascii_num = 0;
					ascii_count = 0;
				}
			}
			else if (ascii_count == 2)
			{
				if (*p == 'w' && ascii_num == ASCII_STATE_RAW)
				{
					ascii_count = 3;
				}
				else if (*p == 'l' && ascii_num == ASCII_STATE_CAL1)
				{
					ascii_count = 3;
				}
				else
				{
					ascii_num = 0;
					ascii_count = 0;
				}
			}
			else if (ascii_count == 3)
			{
				if (*p == ':' && ascii_num == ASCII_STATE_RAW)
				{
					ascii_state = ASCII_STATE_RAW;
					ascii_raw_data_count = 0;
					ascii_num = 0;
					ascii_count = 0;
				}
				else if (*p == '1' && ascii_num == ASCII_STATE_CAL1)
				{
					ascii_count = 4;
				}
				else if (*p == '2' && ascii_num == ASCII_STATE_CAL1)
				{
					ascii_num = ASCII_STATE_CAL2;
					ascii_count = 4;
				}
				else
				{
					ascii_num = 0;
					ascii_count = 0;
				}
			}
			else if (ascii_count == 4)
			{
				if (*p == ':' && ascii_num == ASCII_STATE_CAL1)
				{
					ascii_state = ASCII_STATE_CAL1;
					ascii_raw_data_count = 0;
					ascii_num = 0;
					ascii_count = 0;
				}
				else if (*p == ':' && ascii_num == ASCII_STATE_CAL2)
				{
					ascii_state = ASCII_STATE_CAL2;
					ascii_raw_data_count = 0;
					ascii_num = 0;
					ascii_count = 0;
				}
				else
				{
					ascii_num = 0;
					ascii_count = 0;
				}
			}
			else
			{
				goto fail;
			}
		}
		else if (ascii_state == ASCII_STATE_RAW)
		{
			if (*p == '-')
			{
				// printf("ascii_parse negative\n");
				if (ascii_count > 0)
					goto fail;
				ascii_neg = 1;
			}
			else if (isdigit(*p))
			{
				// printf("ascii_parse digit\n");
				ascii_num = ascii_num * 10 + *p - '0';
				ascii_count++;
			}
			else if (*p == ',')
			{
				// printf("ascii_parse comma, %d\n", ascii_num);
				if (ascii_neg)
					ascii_num = -ascii_num;
				if (ascii_num < -32768 && ascii_num > 32767)
					goto fail;
				if (ascii_raw_data_count >= 8)
					goto fail;
				ascii_raw_data[ascii_raw_data_count++] = ascii_num;
				ascii_num = 0;
				ascii_neg = 0;
				ascii_count = 0;
			}
			else if (*p == 13)
			{
				// printf("ascii_parse newline\n");
				if (ascii_neg)
					ascii_num = -ascii_num;
				if (ascii_num < -32768 && ascii_num > 32767)
					goto fail;
				if (ascii_raw_data_count != 8)
					goto fail;
				ascii_raw_data[ascii_raw_data_count] = ascii_num;
				raw_data(ascii_raw_data);
				ret = 1;
				ascii_raw_data_count = 0;
				ascii_num = 0;
				ascii_neg = 0;
				ascii_count = 0;
				ascii_state = ASCII_STATE_WORD;
			}
			else if (*p == 10)
			{
			}
			else
			{
				goto fail;
			}
		}
		else if (ascii_state == ASCII_STATE_CAL1 || ascii_state == ASCII_STATE_CAL2)
		{
			if (*p == '-')
			{
				// printf("ascii_parse negative\n");
				if (ascii_count > 0)
					goto fail;
				ascii_neg = 1;
			}
			else if (isdigit(*p))
			{
				// printf("ascii_parse digit\n");
				ascii_num = ascii_num * 10 + *p - '0';
				ascii_count++;
			}
			else if (*p == '.')
			{
				// printf("ascii_parse decimal, %d\n", ascii_num);
				if (ascii_raw_data_count > 9)
					goto fail;
				ascii_cal_data[ascii_raw_data_count] = (float)ascii_num;
				ascii_num = 0;
				ascii_count = 0;
			}
			else if (*p == ',')
			{
				// printf("ascii_parse comma, %d\n", ascii_num);
				if (ascii_raw_data_count > 9)
					goto fail;
				ascii_cal_data[ascii_raw_data_count] +=
					(float)ascii_num / powf(10.0f, ascii_count);
				if (ascii_neg)
					ascii_cal_data[ascii_raw_data_count] *= -1.0f;
				ascii_raw_data_count++;
				ascii_num = 0;
				ascii_neg = 0;
				ascii_count = 0;
			}
			else if (*p == 13)
			{
				// printf("ascii_parse newline\n");
				if ((ascii_state == ASCII_STATE_CAL1 && ascii_raw_data_count != 9) || (ascii_state == ASCII_STATE_CAL2 && ascii_raw_data_count != 8))
					goto fail;
				ascii_cal_data[ascii_raw_data_count] +=
					(float)ascii_num / powf(10.0f, ascii_count);
				if (ascii_neg)
					ascii_cal_data[ascii_raw_data_count] *= -1.0f;
				if (ascii_state == ASCII_STATE_CAL1)
				{
					cal1_data(ascii_cal_data);
				}
				else if (ascii_state == ASCII_STATE_CAL2)
				{
					cal2_data(ascii_cal_data);
				}
				ret = 1;
				ascii_raw_data_count = 0;
				ascii_num = 0;
				ascii_neg = 0;
				ascii_count = 0;
				ascii_state = ASCII_STATE_WORD;
			}
			else if (*p == 10)
			{
			}
			else
			{
				goto fail;
			}
		}
	}
	return ret;
fail:
	// printf("ascii FAIL\n");
	ascii_state = ASCII_STATE_WORD;
	ascii_raw_data_count = 0;
	ascii_num = 0;
	ascii_neg = 0;
	ascii_count = 0;
	return 0;
}

static void newdata(const unsigned char *data, int len)
{
	packet_parse(data, len);
	ascii_parse(data, len);
	// TODO: learn which one and skip the other
}

// PhiEs start ======= 
void clear_file(const char *filename) {
	FILE *file = fopen(filename, "w");
	if (file != NULL)
	{
		fclose(file);
	}
}

int read_ipc_file_data(const char *filename)
{
	char buf[256];
	FILE *inFile = fopen(filename, "rb");
	if(inFile) {
		if (fgets(buf,sizeof(buf),inFile) != NULL)
		{
			clear_file(filename);
			// Conver to uchar
			unsigned char uchar_data[256];
			for (size_t i = 0; i < strlen(buf); i++) {
				uchar_data[i] = (unsigned char)buf[i];
			}
			uchar_data[strlen(buf)] = '\0';
			newdata(uchar_data, sizeof(uchar_data));
		}
		fclose(inFile);
		return sizeof(buf);
	}
	return 0;
}

static char result_filename[512];
int write_ipc_file_data(const void *ptr, int len)
{
	FILE *file = fopen(result_filename, "wb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    int written = fwrite(ptr, 1, len, file);
    if (written < len) {
        perror("Error writing to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return written;
}

// PhiEs end ======= 

void set_result_filename(const char *filename) {
	strncpy(result_filename, filename, sizeof(result_filename) - 1);
    result_filename[sizeof(result_filename) - 1] = '\0';
}
