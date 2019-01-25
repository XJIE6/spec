code* eval(state* st, code* instruction) {
    switch(instruction->number) {

        case 0x00:
            return add_01_8(st, instruction);
        break;

        case 0x01:
            if (REXW(instruction->REX)) {
                return add_01_64(st, instruction);
            }
            else {
                return add_01_32(st, instruction);
            }
        break;

        case 0x02:
            return add_03_8(st, instruction);
        break;

        case 0x03:
            if (REXW(instruction->REX)) {
                return add_03_64(st, instruction);
            }
            else {
                return add_03_32(st, instruction);
            }
        break;

        case 0x04:
            return add_04_8(st, instruction);
        break;

        case 0x05:
            if (REXW(instruction->REX)) {
                return add_05_64(st, instruction);
            }
            else {
                return add_05_32(st, instruction);
            }
        break;


    }
}