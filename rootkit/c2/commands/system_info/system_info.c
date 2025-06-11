#include "system_info.h"
#include <linux/utsname.h>
#include <linux/cpu.h>
#include <asm/processor.h>
#include <linux/delay.h>
#include <linux/kernel_stat.h>
#include <linux/sysinfo.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/dmi.h>
#include <linux/usb.h>


// нет проверки на переполнение буф отправки


// ==============================================================================
// =                                 os_info                                    =
// ==============================================================================
/*
    truct new_utsname {
	char sysname[__NEW_UTS_LEN + 1];    - Название операционной системы
	char nodename[__NEW_UTS_LEN + 1];   - Имя хоста
	char release[__NEW_UTS_LEN + 1];    - Версия ядра
	char version[__NEW_UTS_LEN + 1];    - Дополнительная информация о сборке ядра
	char machine[__NEW_UTS_LEN + 1];    - Архитектура процессора
	char domainname[__NEW_UTS_LEN + 1]; - Доменное имя сети
    };
*/
void os_info(struct kvec *vec) {
    struct new_utsname *uts = utsname();
    char *ptr = vec->iov_base;

    if (strlen(uts->sysname)) {
        strcpy(ptr, uts->sysname); ptr += strlen(uts->sysname) + 1;
    } else {
        *ptr++ = '\xFF';
    }

    if (strlen(uts->nodename)) {
        strcpy(ptr, uts->nodename); ptr += strlen(uts->nodename) + 1;
    } else {
        *ptr++ = '\xFF';
    }

    if (strlen(uts->release)) {
        strcpy(ptr, uts->release); ptr += strlen(uts->release) + 1;
    } else {
        *ptr++ = '\xFF';
    }

    if (strlen(uts->version)) {
        strcpy(ptr, uts->version); ptr += strlen(uts->version) + 1;
    } else {
        *ptr++ = '\xFF';
    }

    if (strlen(uts->machine)) {
        strcpy(ptr, uts->machine); ptr += strlen(uts->machine) + 1;
    } else {
        *ptr++ = '\xFF';
    }

    if (strlen(uts->domainname)) {
        strcpy(ptr, uts->domainname); ptr += strlen(uts->domainname) + 1;
    } else {
        *ptr++ = '\xFF';
    }

    *ptr = '\0';
    vec->iov_len = ptr - (char *)vec->iov_base;
    return;
}

// ==============================================================================
// =                             hardware_info                                  =
// ==============================================================================
/*
struct cpuinfo_x86 {
    // Основные характеристики CPU
	union {                     
		struct {
			__u8	x86_model;                                  - Модель процессора
			__u8	x86;                                        - Семейство процессора
			__u8	x86_vendor;                                 - Индекс производителя (x86_vendor_id)
			__u8	x86_reserved;                               - Зарезервированное поле
		};
		__u32		x86_vfm;                                    - Объединённое значение vendor+family+model
	};

    // Другие параметры
	__u8			x86_stepping;                               - Ревизия (stepping) ядра процессора
#ifdef CONFIG_X86_64
	int			x86_tlbsize;                                    - Кол-во страниц (в 4КБ) в объединённой DTLB/ITLB. Только для 64-битных систем
#endif
#ifdef CONFIG_X86_VMX_FEATURE_NAMES
	__u32			vmx_capability[NVMXINTS];                   - Возможности виртуализации (если включена поддержка VMX)
#endif
	__u8			x86_virt_bits;                              - Количество битов в виртуальном адресе
	__u8			x86_phys_bits;                              - Количество битов в физическом адресе
	__u32			extended_cpuid_level;                       - Максимальный номер расширенной функции CPUID
	int			cpuid_level;                                    - Максимальный поддерживаемый уровень CPUID (если -1, то CPUID недоступен)

    // Возможности CPU
	union {
		__u32		x86_capability[NCAPINTS + NBUGINTS];        - Массив битов с флагами возможностей процессора
		unsigned long	x86_capability_alignment;               - Используется в сочетании с макросами, например cpu_has(&c, X86_FEATURE_SSE2)
	};

    // Строковые идентификаторы
	char			x86_vendor_id[16];                          - Строка вендора (например "GenuineIntel" или "AuthenticAMD")
	char			x86_model_id[64];                           - Строка с полной моделью

    // Топология CPU
	struct cpuinfo_topology	topo;                               - Структура с информацией о NUMA, ядрах, потоках и т.д.

    // Кэш и QoS
	unsigned int		x86_cache_size;                         - Размер кеша в Кб
	int			x86_cache_alignment;	                        - Выравнивание кэш-линий в байтах
	int			x86_cache_max_rmid;	                            - Максимальное число доступных RMID (Resource Monitoring ID), т.е. сколько потоков можно отслеживать одновременно
	int			x86_cache_occ_scale;	                        - Шкала для перевода RMID usage → в байты
	int			x86_cache_mbm_width_offset;                     - Смещение для ширины MBM-счётчиков (Memory Bandwidth Monitoring)

    // Энергопитание и производительность
	int			x86_power;                                      - Энергосберегающие характеристики
	unsigned long		loops_per_jiffy;                        - Количество циклов, которое может выполняться за один таймерный тик

    // Защищённый ID и прочее
	u64			ppin;                                           - Protected Processor Identification Number (аналог serial number)
	u16			x86_clflush_size;                               - Размер области, которую очищает инструкция CLFLUSH
	u16			booted_cores;                                   - Сколько ядер инициализировано ОС
	u16			cpu_index;                                      - Индекс CPU в массиве per_cpu
	bool			smt_active;                                 - Включен ли SMT (гиперпоточность)
	u32			microcode;                                      - Версия микрокода процессора
	u8			x86_cache_bits;                                 - Сколько битов используется для индексации в кеш
	unsigned		initialized : 1;                            - Флаг, указывающий, что структура инициализирована
} __randomize_layout;
*/
static void hw_cpu_info(void) {
    struct cpuinfo_x86 *c;
    unsigned int cpu = smp_processor_id();
    c = &cpu_data(cpu);
    pr_info("=== CPU Info ===\n");
    pr_info("Vendor:       %s\n", c->x86_vendor_id);
    pr_info("Model name:   %s\n", c->x86_model_id);
    pr_info("Family:       %u\n", c->x86);
    pr_info("Model:        %u\n", c->x86_model);
    pr_info("Stepping:     %u\n", c->x86_stepping);
    pr_info("Microcode:    0x%x\n", c->microcode);
    pr_info("Logical CPUs: %u\n", num_online_cpus());
}

/*
    enum dmi_field {
        DMI_NONE,                   - Нет значения (по умолчанию)
        DMI_BIOS_VENDOR,            - Производитель BIOS
        DMI_BIOS_VERSION,           - Версия BIOS
        DMI_BIOS_DATE,              - Дата выпуска BIOS
        DMI_BIOS_RELEASE,           - Номер релиза BIOS (не всегда заполняется)
        DMI_EC_FIRMWARE_RELEASE,    - Версия прошивки встроенного контроллера (EC - Embedded Controller)
        DMI_SYS_VENDOR,             - Производитель системы
        DMI_PRODUCT_NAME,           - Название продукта (модель устройства)
        DMI_PRODUCT_VERSION,        - Версия продукта (иногда используется в OEM-устройствах)
        DMI_PRODUCT_SERIAL,         - Серийный номер устройства
        DMI_PRODUCT_UUID,           - UUID системы (обычно уникален на каждой машине)
        DMI_PRODUCT_SKU,            - SKU (Stock Keeping Unit) — внутренний код модели
        DMI_PRODUCT_FAMILY,         - Семейство продукта
        DMI_BOARD_VENDOR,           - Производитель системной платы
        DMI_BOARD_NAME,             - Название/модель платы
        DMI_BOARD_VERSION,          - Версия платы
        DMI_BOARD_SERIAL,           - Серийный номер платы
        DMI_BOARD_ASSET_TAG,        - Asset Tag — инвентарный номер, назначаемый в организациях
        DMI_CHASSIS_VENDOR,         - Производитель корпуса
        DMI_CHASSIS_TYPE,           - Тип корпуса (например, ноутбук, десктоп, сервер и т.п.)
        DMI_CHASSIS_VERSION,        - Версия корпуса
        DMI_CHASSIS_SERIAL,         - Серийный номер корпуса
        DMI_CHASSIS_ASSET_TAG,      - Asset Tag для корпуса
        DMI_STRING_MAX,             - Количество стандартных строк DMI (ограничитель массива)
        DMI_OEM_STRING,             - OEM-строки — строки, добавленные производителем. Особый случай: не входит в dmi_ident
    };
*/
static const char *dmi_field_names[] = {
    "DMI_BIOS_VENDOR",
    "DMI_BIOS_VERSION",
    "DMI_BIOS_DATE",
    "DMI_BIOS_RELEASE",
    "DMI_EC_FIRMWARE_RELEASE",
    "DMI_SYS_VENDOR",
    "DMI_PRODUCT_NAME",
    "DMI_PRODUCT_VERSION",
    "DMI_PRODUCT_SERIAL",
    "DMI_PRODUCT_UUID",
    "DMI_PRODUCT_SKU",
    "DMI_PRODUCT_FAMILY",
    "DMI_BOARD_VENDOR",
    "DMI_BOARD_NAME",
    "DMI_BOARD_VERSION",
    "DMI_BOARD_SERIAL",
    "DMI_BOARD_ASSET_TAG",
    "DMI_CHASSIS_VENDOR",
    "DMI_CHASSIS_TYPE",
    "DMI_CHASSIS_VERSION",
    "DMI_CHASSIS_SERIAL",
    "DMI_CHASSIS_ASSET_TAG",
};

static void hw_dmi_info(void) {
    int i;
    const char *info;

    pr_info("=== DMI Info ===\n");
    for (i = DMI_BIOS_VENDOR; i < DMI_STRING_MAX; ++i) {
        info = dmi_get_system_info(i);
        if (info && *info)
            pr_info("%s: %s\n", dmi_field_names[i], info);
        else
            pr_info("%s: <not available>\n", dmi_field_names[i]);
    }

    return;
}

// usb_for_each_dev(NULL, hw_usb_info); - чтобы вызвать 
static int hw_usb_info(struct usb_device *udev, void *data) {
    pr_info("USB Device Info:\n");
    pr_info("  Bus: %03d Device: %03d\n", udev->bus->busnum, udev->devnum);
    pr_info("  Vendor ID: 0x%04x\n", le16_to_cpu(udev->descriptor.idVendor));
    pr_info("  Product ID: 0x%04x\n", le16_to_cpu(udev->descriptor.idProduct));
    pr_info("  Device Version: %x.%02x\n",
            udev->descriptor.bcdDevice >> 8, udev->descriptor.bcdDevice & 0xff);

    if (udev->manufacturer)
        pr_info("  Manufacturer: %s\n", udev->manufacturer);
    if (udev->product)
        pr_info("  Product: %s\n", udev->product);
    if (udev->serial)
        pr_info("  Serial Number: %s\n", udev->serial);

    pr_info("  Device Class: 0x%02x\n", udev->descriptor.bDeviceClass);
    pr_info("  Device SubClass: 0x%02x\n", udev->descriptor.bDeviceSubClass);
    pr_info("  Device Protocol: 0x%02x\n", udev->descriptor.bDeviceProtocol);

    pr_info("  USB Version: %x.%02x\n",
            udev->descriptor.bcdUSB >> 8, udev->descriptor.bcdUSB & 0xff);

    pr_info("  Speed: %s\n", 
            udev->speed == USB_SPEED_LOW ? "1.5 Mbps (Low Speed)" :
            udev->speed == USB_SPEED_FULL ? "12 Mbps (Full Speed)" :
            udev->speed == USB_SPEED_HIGH ? "480 Mbps (High Speed)" :
            udev->speed == USB_SPEED_SUPER ? "5 Gbps (SuperSpeed)" :
            udev->speed == USB_SPEED_SUPER_PLUS ? "10 Gbps (SuperSpeed+)" : "Unknown");

    pr_info("  Number of Configurations: %d\n", udev->actconfig ? udev->actconfig->desc.bNumInterfaces : 0);
        
    return 0;
}

#include <linux/blkdev.h>
#include <linux/device.h>
static int my_block_device_cb(struct device *dev, void *data)
{
	struct gendisk *gd = dev_to_disk(dev);
    // printk(KERN_INFO "Block device: %s\n", dev_name(dev));
	pr_info("Disk: %s\n", gd->disk_name);
	pr_info("  Size: %llu bytes\n", div64_u64((u64)get_capacity(gd) << 9, 1024ULL * 1024 * 1024));
    return 0;
}

// block_class - не экспортирован, надо будет через kallsyms найти адрес
static void hw_storage_info(void) {
    printk(KERN_INFO "Iterating block devices:\n");

    // Итерируем устройства внутри класса
    class_for_each_device(block_class_ptr, NULL, NULL, my_block_device_cb);
    return;
}

#include <linux/input.h>
static struct list_head *input_dev_list = NULL; // недоступен
spinlock_t *input_mutex = NULL; // для защиты списка
void hardware_info(void) {
	struct input_dev *dev;
    pr_info("  Phys: %p\n", input_dev_list);

    // spin_lock(input_mutex);
    // list_for_each_entry(dev, input_dev_list, node) {
	// 	const char *type = get_device_type(dev);
    //     pr_info("Input Device: %s\n", dev->name);
    //     pr_info("  Type dont know how ident\n");
    //     pr_info("  Phys: %s\n", dev->phys);
    //     pr_info("  Unique ID: %s\n", dev->uniq);
    //     pr_info("  Vendor: 0x%04x\n", dev->id.vendor);
    //     pr_info("  Product: 0x%04x\n", dev->id.product);
    //     pr_info("  Version: 0x%04x\n", dev->id.version);
    //     pr_info("  Bus type: %d\n", dev->id.bustype);
    //     // Можно добавить вывод поддерживаемых событий и других полей
    // }
    // spin_unlock(input_mutex);
    return;
}

// ==============================================================================
// =                               system_load                                  =
// ==============================================================================
// NET
// #include <linux/netdevice.h>
// #include <linux/rtnetlink.h>

// ============================== cpu_load ======================================
/*  
    6.12.25-amd64
    enum cpu_usage_stat {
        CPUTIME_USER,               - время выполнения процессов в режиме пользователя
        CPUTIME_NICE,               - время выполнения процессов с приоритетом nice в режиме пользователя
        CPUTIME_SYSTEM,             - время выполнения процессов в режиме ядра
        CPUTIME_SOFTIRQ,            - время обработки программных прерываний
        CPUTIME_IRQ,                - время обработки аппаратных прерываний
        CPUTIME_IDLE,               - время простоя CPU
        CPUTIME_IOWAIT,             - время ожидания I/O операций
        CPUTIME_STEAL,              - время, которое используют другие операционные системы (при виртуализации)
        CPUTIME_GUEST,              - время выполнения «гостевых» процессов (при виртуализации)
        CPUTIME_GUEST_NICE,         - время выполнения «гостевых» процессов с приоритетом nice (при виртуализации)
    #ifdef CONFIG_SCHED_CORE
        CPUTIME_FORCEIDLE,          - время вынужденного простоя CPU
    #endif
        NR_STATS,                   - перечисление не завершено
    };
*/
#define SLEEP_DELAY 1000                // ms
#define MAX_CPUS 128
struct cpu_stats {
    u64 user, nice, system, softirq, irq, idle, iowait, steal, guest, guest_nice;
    #ifdef CONFIG_SCHED_CORE
        u64 forceidle;
    #endif
};
static struct cpu_stats stat_before[MAX_CPUS];
static struct cpu_stats stat_after[MAX_CPUS];

static void collect_cpu_stat(int cpu, struct cpu_stats *out)
{
    struct kernel_cpustat *kcs = &kcpustat_cpu(cpu);

    out->user = kcs->cpustat[CPUTIME_USER];
    out->nice = kcs->cpustat[CPUTIME_NICE];
    out->system = kcs->cpustat[CPUTIME_SYSTEM];
    out->softirq = kcs->cpustat[CPUTIME_SOFTIRQ];
    out->irq = kcs->cpustat[CPUTIME_IRQ];
    out->idle = kcs->cpustat[CPUTIME_IDLE];
    out->iowait = kcs->cpustat[CPUTIME_IOWAIT];
    out->steal = kcs->cpustat[CPUTIME_STEAL];
    out->guest = kcs->cpustat[CPUTIME_GUEST];
    out->guest_nice = kcs->cpustat[CPUTIME_GUEST_NICE];
#ifdef CONFIG_SCHED_CORE
    out->forceidle = kcs->cpustat[CPUTIME_FORCEIDLE];
#endif
}

static void cpu_load(void){
    int cpu;
    u64 total_used_before = 0, total_total_before = 0;
    u64 total_used_after = 0, total_total_after = 0;

    for_each_online_cpu(cpu)
        collect_cpu_stat(cpu, &stat_before[cpu]);

    msleep(SLEEP_DELAY); 

    for_each_online_cpu(cpu)
        collect_cpu_stat(cpu, &stat_after[cpu]);

    for_each_online_cpu(cpu) {
        u64 used_before, total_before;
        u64 used_after, total_after;
        u64 delta_used, delta_total;
        u64 usage = 0;

        used_before =
            stat_before[cpu].user +
            stat_before[cpu].nice +
            stat_before[cpu].system +
            stat_before[cpu].irq +
            stat_before[cpu].softirq +
            stat_before[cpu].steal +
            stat_before[cpu].guest +
            stat_before[cpu].guest_nice;

        total_before = used_before + stat_before[cpu].idle + stat_before[cpu].iowait;
    #ifdef CONFIG_SCHED_CORE
        total_before += stat_before[cpu].forceidle;
    #endif

        used_after =
            stat_after[cpu].user +
            stat_after[cpu].nice +
            stat_after[cpu].system +
            stat_after[cpu].irq +
            stat_after[cpu].softirq +
            stat_after[cpu].steal +
            stat_after[cpu].guest +
            stat_after[cpu].guest_nice;

        total_after = used_after + stat_after[cpu].idle + stat_after[cpu].iowait;
    #ifdef CONFIG_SCHED_CORE
        total_before += stat_after[cpu].forceidle;
    #endif

        delta_used = used_after - used_before;
        delta_total = total_after - total_before;

        total_used_before += used_before;
        total_total_before += total_before;
        total_used_after += used_after;
        total_total_after += total_after;

        if (delta_total != 0)
            usage = (delta_used * 100) / delta_total;

        pr_info("CPU %d usage: %llu%%\n", cpu, usage);
    }

    u64 total_delta_used = total_used_after - total_used_before;
    u64 total_delta_total = total_total_after - total_total_before;
    u64 total_usage = 0;

    if (total_delta_total != 0)
        total_usage = (total_delta_used * 100) / total_delta_total;

    pr_info("Total CPU usage: %llu%%\n", total_usage);
    return;
}
// ==============================================================================


// ============================== ram_load ======================================
/*
    struct sysinfo {
	__kernel_long_t uptime;		- количество секунд, прошедшее с загрузки системы
	__kernel_ulong_t loads[3];	- средняя одно-, пяти- и пятнадцатиминутная загружённость системы
	__kernel_ulong_t totalram;	- общий объём доступной оперативной памяти
	__kernel_ulong_t freeram;	- объём свободной памяти ram;
	__kernel_ulong_t sharedram;	- Память, используемая разделяемыми (shared) ресурсами
	__kernel_ulong_t bufferram;	- Память, занятая буферами (кешем файловой системы).
	__kernel_ulong_t totalswap;	- Общий объём swap-памяти (подкачки)
	__kernel_ulong_t freeswap;	- Свободный swap (доступен для использования).
	__u16 procs;		   	    - Количество активных процессов.
	__u16 pad;		   	        - Выравнивание (для совместимости с архитектурой m68k).
	__kernel_ulong_t totalhigh;	- Общий объём high memory (память > 896 МБ в старых ядрах x86).
	__kernel_ulong_t freehigh;	- Свободная high memory.
	__u32 mem_unit;			    - Размер единицы памяти
	char _f[20-2*sizeof(__kernel_ulong_t)-sizeof(__u32)];	Padding: libc5 uses this.. 
    };
*/
static void ram_load(void) {
    struct sysinfo info;
    si_meminfo(&info);
    // si_swapinfo(&info); надо будет найти адрес этой функции

    unsigned long total = info.totalram * info.mem_unit;
    unsigned long free = info.freeram * info.mem_unit;
    unsigned long buffers = info.bufferram * info.mem_unit;
    unsigned long shared = info.sharedram * info.mem_unit;

    unsigned long cached = (global_node_page_state(NR_FILE_PAGES)
                            - global_node_page_state(NR_SHMEM)
                            - total_swapcache_pages()) * PAGE_SIZE;

    unsigned long used = total - free - buffers - cached;
    unsigned long buff_cache = buffers + cached;

    unsigned long swap_total = info.totalswap * info.mem_unit;
    unsigned long swap_free = info.freeswap * info.mem_unit;
    unsigned long swap_used = swap_total - swap_free;

    pr_info("== Free -h style ==\n");
    pr_info("MemTotal:       %lu MB\n", total / 1024 / 1024);
    pr_info("MemUsed:        %lu MB\n", used / 1024 / 1024);
    pr_info("MemFree:        %lu MB\n", free / 1024 / 1024);
    pr_info("MemShared:      %lu MB\n", shared / 1024 / 1024);
    pr_info("MemBuff/Cache:  %lu MB\n", buff_cache / 1024 / 1024);

    pr_info("SwapTotal:      %lu MB\n", swap_total / 1024 / 1024);
    pr_info("SwapUsed:       %lu MB\n", swap_used / 1024 / 1024);
    pr_info("SwapFree:       %lu MB\n", swap_free / 1024 / 1024);
    return;
}
// ==============================================================================


// =========================== disks_load =======================================
/*
    struct kstatfs {
        long f_type;
        long f_bsize;
        u64 f_blocks;
        u64 f_bfree;
        u64 f_bavail;
        u64 f_files;
        u64 f_ffree;
        __kernel_fsid_t f_fsid;
        long f_namelen;
        long f_frsize;
        long f_flags;
        long f_spare[4];
    };
*/
static void disks_load(void) {
    
}
// ==============================================================================


void system_load(void) {
    hardware_info();
    return;
}