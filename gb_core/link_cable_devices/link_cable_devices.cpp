
include "../gb.h"


void link_multi_port_device::send_byte(byte which, byte dat)
{
	byte ret = v_gb[which]->get_cpu()->seri_send(dat);

	if (which == 0) log_traffic(0, dat);
	log_traffic(which + 1, ret);

}

void link_multi_port_device::broadcast_byte(byte dat)
{
	for (byte i = 0x00; i < (byte)v_gb.size(); i++)
	{
		send_byte(i, dat);
	}
}

void link_multi_port_device::log_traffic(byte id, byte b) {
	std::string filePath = "./4p_log.csv";
	std::ofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
	if (id < 4)
	{
		ofs << "" << std::hex << (int)b << ",";

	}
	else
	{
		ofs << "" << std::hex << (int)b << std::endl;
	}

	ofs.close();
};

//these are all cheating functions cause, that's nothing real link hardware could do,
//but helpful to get a hack working 

bool link_multi_port_device::all_IE_are_handled();


void link_multi_port_device::get_all_SC_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SC;
		mem.in_data_buffer[i] = data;
	}
};

void link_multi_port_device::get_all_SB_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SB;
		mem.in_data_buffer[i] = data;
	}
};

bool link_multi_port_device::is_expected_data(byte data)
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if (mem.in_data_buffer[i] != data) return false;
	}
	return true;
}

bool link_multi_port_device::is_ready_to_process() {

	get_all_SC_reg_data();
	return is_expected_data(0x80) && all_IE_are_handled();
}

bool link_multi_port_device::all_IE_are_handled()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if ((v_gb[i]->get_regs()->IF & v_gb[i]->get_regs()->IE & INT_SERIAL)) return false;
	}
	return true;

}

