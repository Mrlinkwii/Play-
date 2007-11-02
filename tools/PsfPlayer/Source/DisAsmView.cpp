#include "DisAsmView.h"
#include "CursesUtils.h"
#include "countof.h"

CDisAsmView::CDisAsmView(CMIPS& cpu) :
m_cpu(cpu),
m_viewAddress(0)
{

}

CDisAsmView::~CDisAsmView()
{

}

void CDisAsmView::SetViewAddress(uint32 viewAddress)
{
    m_viewAddress = viewAddress;
}

void CDisAsmView::EnsureVisible(uint32 address, WINDOW* window)
{
    int height, width;
    getmaxyx(window, height, width);
    if((address < m_viewAddress) || (address >= m_viewAddress + (height * 4)))
    {
        SetViewAddress(address);
    }
}

void CDisAsmView::Update(WINDOW* window, int width, int height)
{
    uint32 address = m_viewAddress;
    getmaxyx(window, height, width);

    for(int i = 0; i < height; i++)
    {
        if(address == m_cpu.m_State.nPC)
        {
            wattr_on(window, COLOR_PAIR(3), NULL);
        }

        wmove(window, i, 0);
        CursesUtils::ClearLine(window, width);

        char mnemonic[256];
        char operands[256];
        uint32 opcode = m_cpu.m_pMemoryMap->GetWord(address);
        m_cpu.m_pArch->GetInstructionMnemonic(&m_cpu, address, opcode, mnemonic, countof(mnemonic));
        m_cpu.m_pArch->GetInstructionOperands(&m_cpu, address, opcode, operands, countof(operands));

        wmove(window, i, 0);
        wprintw(window, "%0.8X", address);

        wmove(window, i, 10);
        wprintw(window, "%0.8X", opcode);

        wmove(window, i, 20);
        wprintw(window, mnemonic);
        wmove(window, i, 30);
        wprintw(window, operands);

        if(address == m_cpu.m_State.nPC)
        {
            wattr_off(window, COLOR_PAIR(3), NULL);
        }

        address += 4;
    }

    wrefresh(window);
}
