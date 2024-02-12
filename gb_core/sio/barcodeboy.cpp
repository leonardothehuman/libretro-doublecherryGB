#include "barcodeboy.hpp"

std::vector<std::string> battle_space_barcodes{
	"4907981000301", //Berserker
	"4908052808369", //Valkyrie
	"4916911302309", //Grizzly Bear
	"4902776809367", //Magic Soldier
	"4905672306367", //Knight		
	"4912713004366", //Wraith
	"4913508504399", //Shaman		
	"4918156001351", //Thier
	"4911826551347", //Sorcerer		
	"4909062206350" //Warrior
};


barcodeboy::barcodeboy(std::vector<gb*> g_gb) {

	v_gb.insert(v_gb.begin(), std::begin(g_gb), std::end(g_gb));
	this->transfer_speed = 512*14;
	v_gb[0]->set_linked_target(this);
	reset();
}


void barcodeboy::reset() {
	barcode_index = 0;
	current_barcode = battle_space_barcodes[this->barcode_index];
	seri_send_counter = 0;
	round = 0;
	is_in_mastermode = false; 
}

void barcodeboy::process() {

	if (!is_in_mastermode) return;
	if (!is_ready_for_next_tik()) return; 
	
	update_seri_occer(); 

	if (!seri_send_counter++) {
		send_byte(0, 0x02);
		return; 
	}

	if (seri_send_counter < 15) {
		byte next = this->current_barcode.at((seri_send_counter - 2));
		send_byte(0, next);
		return; 
	}

	send_byte(0, 0x03);
	seri_send_counter = 0;
	if (round++) {
		is_in_mastermode = false;
		round = 0; 
		barcode_index = ++barcode_index >= (battle_space_barcodes.size()) ? 0 : barcode_index;
		current_barcode = battle_space_barcodes[barcode_index];
	}

}


byte barcodeboy::seri_send(byte data) 
{
	log_traffic(0, data);
	if (is_in_mastermode) return 0xFF;
	seri_send_counter++;

	if (seri_send_counter < 3) return 0xFF;
	if (seri_send_counter == 3) return 0x10;

	seri_send_counter = 0;
	is_in_mastermode = true; 
	update_seri_occer();

	return 0x07;

}

byte barcodeboy::get_SB_value() { return 0xFF; };
byte barcodeboy::get_SC_value() { return is_in_mastermode ? 0x81 : 0x80; };


void barcodeboy::serialize(serializer& s)
{

	//s_VAR(this->seri_send_counter);
	s_VAR(this->round);
	s_VAR(this->is_in_mastermode);

	//s_ARRAY(this->current_barcode.data())
	//s_ARRAY(in_data_buffer);
	//s_VAR(process_occer);


}