
#include "link_master_device.hpp"

extern bool logging_allowed;

byte link_master_device::send_byte(byte which, byte dat)
{
	byte ret = v_gb[which]->get_cpu()->seri_send(dat);

	if (which == 0) log_traffic(0, dat);
	log_traffic(which + 1, ret);

	return ret; 
}

void link_master_device::broadcast_byte(byte dat)
{
	for (byte i = 0x00; i < (byte)v_gb.size(); i++)
	{
		send_byte(i, dat);
	}
}

void link_master_device::log_traffic(byte id, byte b) {

	if (logging_allowed) {
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
	}
};

bool link_master_device::is_ready_for_next_tik() {
	return v_gb[0]->get_cpu()->get_clock() >= seri_occer; 
}

void link_master_device::update_seri_occer() {
	seri_occer = v_gb[0]->get_cpu()->get_clock() + transfer_speed;
}


size_t link_master_device::get_state_size(void)
{
	size_t ret = 0;
	serializer s(&ret, serializer::COUNT);
	serialize(s);
	return ret;
}

void link_master_device::save_state_mem(void* buf)
{
	serializer s(buf, serializer::SAVE_BUF);
	serialize(s);
}

void link_master_device::restore_state_mem(void* buf)
{
	serializer s(buf, serializer::LOAD_BUF);
	serialize(s);
}


//these are all cheating functions cause, that's nothing real link hardware could do,
//but helpful to get a hack working 


bool link_master_device::all_IE_are_handled()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if ((v_gb[i]->get_regs()->IF & v_gb[i]->get_regs()->IE & INT_SERIAL)) return false;
	}
	return true;

}

void link_master_device::get_all_SC_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SC;
		in_data_buffer[i] = data;
	}
};

void link_master_device::get_all_SB_reg_data()
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		byte data = v_gb[i]->get_regs()->SB;
		in_data_buffer[i] = data;
	}
};

bool link_master_device::is_expected_data(byte data)
{
	for (int i = 0; i < v_gb.size(); i++)
	{
		if (in_data_buffer[i] != data) return false;
	}
	return true;
}

bool link_master_device::gbs_are_ready_to_process() {

	get_all_SC_reg_data();
	return is_expected_data(0x80) && all_IE_are_handled();
}
