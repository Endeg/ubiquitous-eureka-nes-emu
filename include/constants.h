#ifndef _EMU_CONSTANTS_H
#define _EMU_CONSTANTS_H

#define PpuRegisterAddressStart (0x2000)
#define PpuRegisterAddressEnd   (0x3FFF)
#define PpuRegisterCount        (0x0008)

#define PPUCTRL   (0x0000)
#define PPUMASK   (0x0001)
#define PPUSTATUS (0x0002)
#define OAMADDR   (0x0003)
#define OAMDATA   (0x0004)
#define PPUSCROLL (0x0005)
#define PPUADDR   (0x0006)
#define PPUDATA   (0x0007)
#define OAMDMA    (0x4014)

#define NametableTileSize            (8)
#define NametableTileRowCount        (30)
#define NametableTileTilePerRowCount (32)

#endif
