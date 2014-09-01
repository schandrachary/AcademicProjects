library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all ;
--use ieee.std_logic_unsigned.all;

entity mul is
  port(rst: in std_logic;
    clk: in std_logic; 
    a:in signed(7 downto 0);
    b:in signed(7 downto 0);
    a_shift:out signed(7 downto 0):=(others=>'0');
    b_shift:out signed(7 downto 0):=(others=>'0');
    
    sum: out signed(20 downto 0):=(others=>'0'));
  end mul;
  
  architecture behav of mul is
   signal prod: signed(15 downto 0):=(others=>'0');
 
  signal temp1: signed(20 downto 0):=(others=>'0');
    
    begin
  prod <= a * b; 
  process(rst,clk)
        
        begin
          if(rst='1')then
      --    prod <= (others=>'0');
          sum <= (others=>'0');
          temp1 <= (others=>'0');

        elsif(clk'event and clk='1')then
       -- prod <= a * b;
        sum<=temp1;
      elsif(clk='0')then
        temp1 <= temp1 + prod;
            
        a_shift<=a;
        b_shift<=b;
                       
      
  end if;
   end process;
end behav;
    
