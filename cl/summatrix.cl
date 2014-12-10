#define LOCAL_SIZE 64

__kernel void sum_matrix(__global int * in, __global int * out)
{
    size_t group_id = get_group_id(0);
    int group_r = group_id / LOCAL_SIZE;
    int group_c = group_id % LOCAL_SIZE;
    int sum = 0;
    for(int r = 0; r < LOCAL_SIZE; r++) {
        for(int c = 0; c < LOCAL_SIZE; c++) {
            sum += in[(group_r * LOCAL_SIZE + r) * LOCAL_SIZE * LOCAL_SIZE + group_c * LOCAL_SIZE + c];
        }
    }
    out[group_r * LOCAL_SIZE + group_c] = sum;
}