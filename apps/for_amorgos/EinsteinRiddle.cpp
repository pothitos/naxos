

#include<iostream>
#include<map>
#include<vector>

#include<amorgos.h>
#include<cstdlib>
#include<naxos.h>


using namespace naxos;
using namespace std;


#define var_item(type, item) m_list_name[#type].push_back(#item); NsIntVar item(pm, 0, 4); m_list_var[#type].push_back(item);

class c_who_have_fish
{
private:
	map<string, vector<string>> m_list_name;
	map<string, NsIntVarArray> m_list_var;
public:
	void work()
	{
		NsProblemManager pm;

		var_item(country, country_english);
		var_item(country, country_spanish);
		var_item(country, country_japanese);
		var_item(country, country_ukrainian);
		var_item(country, country_norwegian);

		var_item(pet, pet_snake);
		var_item(pet, pet_zebra);
		var_item(pet, pet_fox);
		var_item(pet, pet_horse);
		var_item(pet, pet_dog);

		var_item(color, color_red);
		var_item(color, color_white);
		var_item(color, color_green);
		var_item(color, color_yellow);
		var_item(color, color_blue);

		var_item(drink, drink_tea);
		var_item(drink, drink_coffee);
		var_item(drink, drink_milk);
		var_item(drink, drink_juice);
		var_item(drink, drink_water);

		var_item(smoke, smoke_winston);
		var_item(smoke, smoke_kool);
		var_item(smoke, smoke_chesterfield);
		var_item(smoke, smoke_lucky);
		var_item(smoke, smoke_kent);

		for (auto& i : m_list_var) pm.add(NsAllDiff(i.second));

		pm.add(country_english == color_red);
		pm.add(country_spanish == pet_dog);
		pm.add(color_green == drink_coffee);
		pm.add(country_ukrainian == drink_tea);
		pm.add((color_green == color_white - 1) || (color_green == color_white + 1));
		pm.add(pet_snake == smoke_winston);
		pm.add(smoke_kool == color_yellow);
		pm.add(drink_milk == 2);
		pm.add(country_norwegian == 0);
		pm.add((pet_fox == smoke_chesterfield - 1) || (pet_fox == smoke_chesterfield + 1));
		pm.add((pet_horse == smoke_kool - 1) || (pet_horse == smoke_kool + 1));
		pm.add(smoke_lucky == drink_juice);
		pm.add(country_japanese == smoke_kent);
		pm.add((country_norwegian == color_blue - 1) || (country_norwegian == color_blue + 1));
		drink_water;
		pet_zebra;

		for (auto& i : m_list_var) pm.addGoal(new NsgLabeling(i.second));

		while (pm.nextSolution() != false)
		{
			map<string, map<int, string>> list_info;

			for (auto& i : m_list_var)
			{
				for (auto k = 0; k < i.second.size(); ++k)
				{
					auto name = m_list_name[i.first][k];
					auto index = m_list_var[i.first][k].value();

					list_info[i.first][index] = name;
				}
			}

			auto func_print_item = [](string str)
			{
				const int fix_len = 20;
				auto diff = fix_len - str.length();
				assert(diff > 0);
				string str_padding(diff, ' ');
				cout << str << "," << str_padding;
			};

			for (auto& i : list_info)
			{
				for (auto& k : i.second)
				{
					func_print_item(k.second);
				}
				cout << endl;
			}
			cout << "-------" << endl;

			cout << "press enter continue" << endl;
			getchar();
		}
	}

private:
	void info()
	{
		/*Houses logical puzzle: who owns the zebra and who drinks water?

		1) Five colored houses in a row, each with an owner, a pet, cigarettes, and a drink.
		2) The English lives in the red house.
		3) The Spanish has a dog.
		4) They drink coffee in the green house.
		5) The Ukrainian drinks tea.
		6) The green house is next to the white house.
		7) The Winston smoker has a serpent.
		8) In the yellow house they smoke Kool.
		9) In the middle house they drink milk.
		10) The Norwegian lives in the first house from the left.
		11) The Chesterfield smoker lives near the man with the fox.
		12) In the house near the house with the horse they smoke Kool.
		13) The Lucky Strike smoker drinks juice.
		14) The Japanese smokes Kent.
		15) The Norwegian lives near the blue house.

		*/
	}
};

int main(int argc, char* argv[])
{
	c_who_have_fish ms;
	ms.work();

	cout << "press enter exit" << endl;
	getchar();
	return 0;
}
