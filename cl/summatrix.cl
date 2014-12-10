#define LOCAL_SIZE 64
#define WORK_GROUP 8

__kernel void sum_matrix(__global int * in, __global int * out)
{
    size_t global_id0 = get_global_id(0);
    size_t global_id1 = get_global_id(1);
    int group_r = global_id0;
    int group_c = global_id1;
    int sum = 0;
    for(int r = 0; r < LOCAL_SIZE; r++) {
        for(int c = 0; c < LOCAL_SIZE; c++) {
            sum += in[(group_r * LOCAL_SIZE + r) * LOCAL_SIZE * WORK_GROUP + group_c * LOCAL_SIZE + c];
        }
    }
    out[group_r * WORK_GROUP + group_c] += sum;
}