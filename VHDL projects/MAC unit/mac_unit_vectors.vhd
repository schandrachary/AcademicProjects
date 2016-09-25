library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all ;
--use ieee.std_logic_unsigned.all;

entity mul_vector is
  port(rst: in std_logic;
    clk: in std_logic; 
    a:in std_logic_vector(7 downto 0);
    b:in std_logic_vector(7 downto 0);
    a_shift:out std_logic_vector(7 downto 0):=(others=>'0');
    b_shift:out std_logic_vector(7 downto 0):=(others=>'0');
    
    sum: out std_logic_vector(20 downto 0):=(others=>'0'));
  end mul_vector;
  
  architecture behav of mul_vector is
   signal prod: signed(15 downto 0):=(others=>'0');
  
  signal temp1: signed(20 downto 0):=(others=>'0');
    signal a1,b1:signed(7 downto 0):=(others=>'0');
    signal sum_store: signed(20 downto 0):=(others=>'0');
    begin
   a1<=signed (a);
   b1<=signed (b) ; 
  prod <= a1 * b1; 
  process(rst,clk)
        
        begin
          if(rst='1')then
      --    prod <= (others=>'0');
          sum_store <= (others=>'0');
          temp1 <= (others=>'0');

        elsif(clk'event and clk='1')then
       -- prod <= a * b;
        sum_store<=temp1;
      elsif(clk='0')then
        temp1 <= temp1 + prod;
         sum <= std_logic_vector(sum_store);   
        a_shift<=std_logic_vector(a1);
        b_shift<=std_logic_vector(b1);
                       
      
  end if;
   end process;
end behav;
    

