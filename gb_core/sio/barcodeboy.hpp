

#include "link_master_device.hpp"
#include <string>


class barcodeboy : public link_target, public link_master_device 
{
	

public:
	barcodeboy(std::vector<gb*> g_gb, char cart_name[18]);
	bool is_in_mastermode; 
	void set_barcodes(char cart_name[18]);
	void process() override;
	void reset() override;
	byte get_SB_value() override;
	byte get_SC_value() override;
	
	void virtual save_state_mem(void* buf) override;
	void virtual restore_state_mem(void* buf) override;
	size_t virtual get_state_size() override;
	
	void virtual serialize(serializer& s) override;
	

	byte seri_send(byte) override;

private:
	int seri_send_counter; 
	int round; 
	int barcode_index;
	std::string current_barcode; 
	std::vector<std::string> barcodes; 
};





