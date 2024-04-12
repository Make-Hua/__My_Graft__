#pragma once
#include <cstdlib>
#include <memory.h>

const int _SIZE = 128;
using u_char = unsigned char;
using ngx_uint_t = unsigned int;

// ����ǰ������
struct ngx_pool_s;

// �����������ͣ��ص�������
typedef void (*ngx_pool_cleanup_pt)(void* data);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt      handler;                             // ����һ������ָ�룬������������Ļص�����
    void                     *data;                               // �ص�����
    ngx_pool_cleanup_s       *next;                               // ���� cleanup ��������һ��������
}; 

struct ngx_pool_large_s {
    ngx_pool_large_s         *next;
    void                     *alloc;
};

// ����С���ڴ�ͷ��������Ϣ
struct ngx_pool_data_t {
    u_char                   *last;                               // С���ڴ�ؿ����ڴ����ʼ��ַ
    u_char                   *end;                                // С���ڴ�ؿ����ڴ��ĩβ��ַ
    ngx_pool_s               *next;                               // ָ����һС���ڴ�
    ngx_uint_t               failed;                              // �ڴ����ʧ�ܵĴ�����ʧ�ܳ���3�ν�ִ����������
} ;

// ngx �ڴ�ص�ͷ����Ϣ�͹����Ա
struct ngx_pool_s {
    ngx_pool_data_t          d;                                   // �洢��ǰС���ڴ�ص�ʹ�����
    size_t                   max;                                 // С���ڴ�ʹ���ڴ�ķֽ���
    ngx_pool_s               *current;                            // ָ���һ���ṩС���ڴ�����С���ڴ��
    ngx_pool_large_s         *large;                              // ����ڴ���������
    ngx_pool_cleanup_s       *cleanup;                            //�ص����������
};


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
// �� b ������ a �ı���
#define ngx_align(d, a) (((d) + (a - 1)) & ~(a - 1))
// ��ָ�� p ������ a ���ٽ�����
#define ngx_align_ptr(p, a) (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))
#define NGX_ALIGNMENT sizeof(unsigned long)
#define ngx_memzero(buf, n) (void) memset(buf, 0, n)

const int ngx_pagesize = 4096;                                    // ����ҳ���С��4K��
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;             // �ڴ�����ɷ����С   ��ҪԤ��һ��ָ��Ŀռ�
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;                      // 16k
const int NGX_POOL_ALIGNMENT = 16;                                // ����16�ֽڶ���
const int NGX_MIN_POOL_SIZE = 
                ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)),
                    NGX_POOL_ALIGNMENT);                          // 

/* OOP ʵ�� nginx �ڴ�ش��� */
class ngx_mem_pool {
public:
    ngx_mem_pool(size_t size = _SIZE);
    ~ngx_mem_pool();

    void* ngx_create_pool(size_t size);                           // ����size��С�ڴ��
    void* ngx_palloc(size_t size);                                // ���ڴ������ size ��С�ڴ棨�������ֽڶ��룩
    void* ngx_pnalloc(size_t size);                               // ���ڴ������ size ��С�ڴ棨�������ֽڶ��룩
    void* ngx_pcalloc(size_t size);                               // ���õľ��� ngx_palloc ʵ���ڴ���䣬���ǻ��ʼ��Ϊ0
    void ngx_pfree(void* p);                                      // ����ڴ��ͷ�
    void ngx_reset_pool();                                        // �ڴ����ú���
    void ngx_destroy_pool();                                      // �ڴ����ٺ���
    ngx_pool_cleanup_s*
        ngx_pool_cleanup_add(size_t size);                        // ��ӻص������������

private:
    ngx_pool_s* pool_;                                            // ָ���ڴ����� 

    void* ngx_palloc_small(size_t size, ngx_uint_t align);        // С���ڴ����
    void* ngx_palloc_large(size_t size);                          // ����ڴ����
    void* ngx_palloc_block(size_t size);                          // �����µ�С���ڴ��
};