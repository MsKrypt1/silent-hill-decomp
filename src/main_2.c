#include "main_2.h"

#define MAX_NAME_LEN 8
#define FILE_ENTRY_PART_SPLIT 4
#define SHIFT_SIZE 6
#define ENCODED_CHAR_SIZE 0x3F
#define ENCODED_CHAR_OFFSET 0x20
#define NO_FILE_TYPE_ID 0xF

void decryptOverlay(s32 *ovl_result, s32 *encrypted_ovl, s32 size) {
  s32 i, seed = 0;

  i = 0;
  while (i < (size >> 2)) {
    seed        = (seed + 0x01309125) * 0x03A452F7;
    *ovl_result = *encrypted_ovl ^ seed;
    i++;
    encrypted_ovl++;
    ovl_result++;
  }
}

s32 getFileEntrySizeBytes(s32 entry_idx) {
  return FILE_ENTRIES_TABLE[entry_idx].blocks_num * FILE_ENTRY_BLOCK_SIZE;
}

void decodeFileEntryNameByOffset(char *out_name, s32 offset) {
  decodeFileEntryName(out_name, FILE_ENTRIES_TABLE + offset);
}

void decodeFileEntryName(char *out_name, FileEntry *file_entry) {
  s32   i = 0;
  char  decoded_char;
  u32   name_part;
  char  file_type_id;
  char *file_type;

  name_part = file_entry->part1;

  while (i < MAX_NAME_LEN) {
    if (i == FILE_ENTRY_PART_SPLIT) {
      name_part = file_entry->part2;
    }

    decoded_char = name_part & ENCODED_CHAR_SIZE;

    if (decoded_char == 0) {
      break;
    }

    out_name[i] = decoded_char + ENCODED_CHAR_OFFSET;
    name_part >>= SHIFT_SIZE;
    i++;
  }

  file_type_id = file_entry->file_type_id;

  if (file_type_id == NO_FILE_TYPE_ID) {
    out_name[i] = NULL;
    return;
  }

  file_type   = FILE_TYPES[file_type_id];
  out_name[i] = *file_type;

  while (*file_type) {
    file_type++;
    i++;
    out_name[i] = *file_type;
  }
}

void encodeFileEntryName(s32 *out_part1, s32 *out_part2, char *name) {
  s32 i;
  s32 current_shift;
  s32 item;

  s32 encoded_char;
  s32 part1_counter;
  s32 part2_counter;

  part1_counter = 0;
  part2_counter = 0;
  current_shift = 0;

  for (i = 0; i < MAX_NAME_LEN; i++) {
    item = name[i];

    if (i == FILE_ENTRY_PART_SPLIT) {
      current_shift = 0;
    }

    if (item == NULL || item == '.') {
      break;
    }

    encoded_char = (item - ENCODED_CHAR_OFFSET) << current_shift;

    if (i < 4) {
      part1_counter |= encoded_char;
    } else {
      part2_counter |= encoded_char;
    }

    current_shift += SHIFT_SIZE;
  }

  *out_part1 = part1_counter;
  *out_part2 = part2_counter;
}

s32 getFileEntrySizeBytesAligned(s32 entry_idx) {
  return ((FILE_ENTRIES_TABLE[entry_idx].blocks_num * FILE_ENTRY_BLOCK_SIZE) +
          (FILE_ENTRY_ALIGNMENT_SIZE - 1)) &
         ~(FILE_ENTRY_ALIGNMENT_SIZE - 1);
}

u32 findFileEntryByFileTypeId(s32 file_type_id, s32 start_offset,
                              s32 direction) {
  s32 i;
  u32 cur_offset;
  s32 increment;

  increment = direction < 0 ? -1 : 1;

  i          = 0;
  cur_offset = start_offset + increment;

  while (i < 0x81A) {
    if (cur_offset >= 0x81A) {
      cur_offset = direction < 0 ? 0x819 : 0;
    }

    if (FILE_ENTRIES_TABLE[cur_offset].file_type_id == file_type_id) {
      return cur_offset;
    }

    cur_offset += increment;
    i++;
  }

  return -1;
}

s32 findFileEntry(u8 *name, s32 file_type_id, s32 start_offset) {
  FileEntry *file_entry;

  s32 part1;
  s32 part2;

  s32 i         = start_offset;
  s32 entry_idx = -1;

  encodeFileEntryName(&part1, &part2, name);

  file_entry = &FILE_ENTRIES_TABLE[i];

  while (i < 0x81A) {
    if (file_entry->part2 == part2 && file_entry->part1 == part1 &&
        file_entry->file_type_id == file_type_id) {
      entry_idx = i;
      break;
    }

    i++;
    file_entry++;
  }

  return entry_idx;
}

s32 FUN_80010e58(s32 arg0) { return arg0 < D_800230A8; }

s32 FUN_80010E68(void) { return D_80022C98[0x100] + 1 - D_80022C98[0x104]; }

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80010e84);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80010ecc);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80010f34);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80010F68);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80010F9C);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80011018);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80011074);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80011170);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_800111dc);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_80011260);

INCLUDE_ASM("asm/main/nonmatchings/main_2", FUN_8001137C);
