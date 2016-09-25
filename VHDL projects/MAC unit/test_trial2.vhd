library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work. ALL;

entity testbench2 is
end entity;

architecture behavioral of testbench2 is
 
component c1 
port
(
  a11,a21,a31,a41,b11,b12,b13			: in signed(7 downto 0):=(others => '0');
		clk			: in std_logic;
		rst1,rst2,rst3,rst4,rst5,rst6		: in std_logic:= '0';
		p11,p12,p13,p21,p22,p23,p31,p32,p33,p41,p42,p43	: out signed (20 downto 0):=(others => '0')
);
	end component;
	
signal a11_test,a21_test,a31_test,a41_test,b11_test,b12_test,b13_test : signed(7 downto 0):=(others => '0');

signal clk_test : std_logic:='1';

signal rst1_test,rst2_test,rst3_test,rst4_test,rst5_test,rst6_test	: std_logic:= '0';

signal p11_test,p12_test,p13_test,p21_test,p22_test,p23_test,p31_test,p32_test,p33_test,p41_test,p42_test,p43_test	: signed (20 downto 0):=(others => '0');

CONSTANT half_clk_period : TIME := 50 ns;
CONSTANT clk_period : TIME := 100 ns;

begin
  process_g0:entity work.mat_mul (behavioral)port map(a11_test,a21_test,a31_test,a41_test,b11_test,b12_test,b13_test,clk_test,rst1_test,rst2_test,rst3_test,rst4_test,rst5_test,rst6_test,p11_test,p12_test,p13_test,p21_test,p22_test,p23_test,p31_test,p32_test,p33_test,p41_test,p42_test,p43_test);
    
    test1 : a11_test <= "00000001" , 
                "00000001" after (clk_period *1.25), 
                "00000001"     after (clk_period *2.25), 
                   "00000001" after (clk_period *3.25), 
                   "00000001" after (clk_period *4.25), 
                   "00000001" after (clk_period *5.25);
 
    test2 : b11_test <= "00000001" , 
                "00000001" after (clk_period *1.25), 
                "00000001"     after (clk_period *2.25), 
                   "00000001" after (clk_period *3.25), 
                   "00000001" after (clk_period *4.25), 
                   "00000001" after (clk_period *5.25);

    test3 : a21_test <= "00000001" after (clk_period *1.25), 
                "00000001"     after (clk_period *2.25), 
                   "00000001" after (clk_period *3.25), 
                   "00000001" after (clk_period *4.25), 
                   "00000001" after (clk_period *5.25),
                   "00000001" after (clk_period *6.25);
                  
    test4 : b12_test <= "00000001" after (clk_period *1.25), 
                "00000001"     after (clk_period *2.25), 
                   "00000001" after (clk_period *3.25), 
                   "00000001" after (clk_period *4.25), 
                   "00000001" after (clk_period *5.25),
                   "00000001" after (clk_period *6.25);
                 
    test5 : a31_test <= "00000001" after (clk_period *2.25), 
                "00000001" after (clk_period *3.25), 
                "00000001"     after (clk_period *4.25), 
                   "00000001" after (clk_period *5.25), 
                   "00000001" after (clk_period *6.25), 
                   "00000001" after (clk_period *7.25);
                  
    test6 : b13_test <="00000001",
                  "00000001" after (clk_period *2.25), 
                "00000001" after (clk_period *3.25), 
                "00000001"     after (clk_period *4.25), 
                    "00000001" after (clk_period *5.25), 
                   "00000001" after (clk_period *6.25), 
                   "00000001" after (clk_period *7.25);
                 
                  
    test7 : a41_test <= "00000001" after (clk_period *3.25)  ,
                "00000001" after (clk_period *4.25), 
                "00000001"     after (clk_period *5.25), 
                   "00000001" after (clk_period *6.25), 
                   "00000001" after (clk_period *7.25), 
                   "00000001" after (clk_period *8.25);
                
    test8 : rst1_test <= '0'  ,
                  '1'  after (clk_period *7.75), 
                      '0'  after (clk_period *8.25); 

    test9 : rst2_test <= '0'  ,
                  '1'  after (clk_period *8.75), 
                      '0'  after (clk_period *9.25); 
                     
    test10 : rst3_test <= '0'  ,
                  '1'  after (clk_period *9.75), 
                      '0'  after (clk_period *10.25); 
                     
    test11 : rst4_test <= '0'  ,
                  '1'  after (clk_period *10.75), 
                      '0'  after (clk_period *11.25); 
                     
    test12 : rst5_test <= '0'  ,
                  '1'  after (clk_period *11.75), 
                      '0'  after (clk_period *12.25); 
                     
    test13 : rst6_test <= '0'  ,
                  '1'  after (clk_period *12.75), 
                      '0'  after (clk_period *13.25); 
                                                                                                                                                          
 process
   begin
     clk_test <= NOT( clk_test);
        wait for half_clk_period;
      end process;
      
end behavioral;
