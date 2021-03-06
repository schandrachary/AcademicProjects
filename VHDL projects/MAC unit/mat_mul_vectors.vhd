library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all ;

entity mat_mul_vector is
  port( 
    
    a11:in std_logic_vector(7 downto 0);
    a21:in std_logic_vector(7 downto 0);
    a31:in std_logic_vector(7 downto 0);
    a41:in std_logic_vector(7 downto 0);
    b11:in std_logic_vector(7 downto 0);
    b12:in std_logic_vector(7 downto 0);
    
    b13:in std_logic_vector(7 downto 0);
    
    
    clk: in std_logic;
    rst1,rst2,rst3,rst4,rst5,rst6: in std_logic;
    
    p11:out std_logic_vector(20 downto 0):=(others=>'0');
    p12:out std_logic_vector(20 downto 0):=(others=>'0');
    p13:out std_logic_vector(20 downto 0):=(others=>'0');
    p21:out std_logic_vector(20 downto 0):=(others=>'0');
     p22:out std_logic_vector(20 downto 0):=(others=>'0');
      p23:out std_logic_vector(20 downto 0):=(others=>'0');
    
    
    p31:out std_logic_vector(20 downto 0):=(others=>'0');
    p32:out std_logic_vector(20 downto 0):=(others=>'0');
    p33:out std_logic_vector(20 downto 0):=(others=>'0');
   
    p41:out std_logic_vector(20 downto 0):=(others=>'0');
   
    
    
    p42:out std_logic_vector(20 downto 0):=(others=>'0');
    p43:out std_logic_vector(20 downto 0):=(others=>'0'));
        
    
  end mat_mul_vector;
  
  architecture behavioral of mat_mul_vector is
    component mul 
      port(rst: in std_logic;
        clk: in std_logic;
    a:in std_logic_vector(7 downto 0);
    b:in std_logic_vector(7 downto 0);
    a_shift:out std_logic_vector(7 downto 0);
    b_shift:out std_logic_vector(7 downto 0);
    
    sum: out std_logic_vector(20 downto 0));
  end component;
  
  signal a_out_p11: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p12: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p21: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p22: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p31: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p32: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p41: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p42: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p11: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p21: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p31: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p12: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p22: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p32: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p13: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p23: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p33: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p13: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p23: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p33: std_logic_vector(7 downto 0):=(others=>'0');
  signal a_out_p43: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p43: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p42: std_logic_vector(7 downto 0):=(others=>'0');
  signal b_out_p41: std_logic_vector(7 downto 0):=(others=>'0');
  
  
  begin
      matop1:mul port map(rst=>rst1,
        clk=>clk,
      a=>a11,
      b=>b11,
      a_shift=>a_out_p11,
      b_shift=>b_out_p11,
      
      sum=>p11);
      
      matop2:mul port map(rst=>rst2,
      clk=>clk,
      a=>a_out_p11,
      b=>b12,
      a_shift=>a_out_p12,
      b_shift=>b_out_p12,
      
      sum=>p12);
      
      matop3:mul port map(rst=>rst2,
      clk=>clk,
      a=>a21,
      b=>b_out_p11,
      a_shift=>a_out_p21,
      b_shift=>b_out_p21,
     
      sum=>p21);
      
      matop4:mul port map(rst=>rst3,
      clk=>clk,
      a=>a_out_p12,
      b=>b13,
      a_shift=>a_out_p13,
      b_shift=>b_out_p13,
      
      sum=>p13);
      
      matop5:mul port map(rst=>rst3,
      clk=>clk,
      a=>a31,
      b=>b_out_p21,
      a_shift=>a_out_p31,
      b_shift=>b_out_p31,
      
      sum=>p31);
      
      matop6:mul port map(rst=>rst3,
      clk=>clk,
      a=>a_out_p21,
      b=>b_out_p12,
      a_shift=>a_out_p22,
      b_shift=>b_out_p22,
      
      sum=>p22);
      
      matop7:mul port map(rst=>rst4,
      clk=>clk,
      a=>a41,
      b=>b_out_p31,
      a_shift=>a_out_p41,
      b_shift=>b_out_p41,
      
      sum=>p41);
      
         
      matop8:mul port map(rst=>rst4,
      clk=>clk,
      a=>a_out_p22,
      b=>b_out_p13,
      a_shift=>a_out_p23,
      b_shift=>b_out_p23,
      
      sum=>p23);
      
      matop9:mul port map(rst=>rst4,
      clk=>clk,
      a=>a_out_p31,
      b=>b_out_p22,
      a_shift=>a_out_p32,
      b_shift=>b_out_p32,
      
      sum=>p32);
      
      matop10:mul port map(rst=>rst5,
      clk=>clk,
      a=>a_out_p32,
      b=>b_out_p23,
      a_shift=>a_out_p33,
      b_shift=>b_out_p33,
      
      sum=>p33);
      
      matop11:mul port map(rst=>rst5,
      clk=>clk,
      a=>a_out_p41,
      b=>b_out_p32,
      a_shift=>a_out_p42,
      b_shift=>b_out_p42,
      
      sum=>p42);
      
      matop12:mul port map(rst=>rst6,
      clk=>clk,
      a=>a_out_p42,
      b=>b_out_p33,
      a_shift=>a_out_p43,
      b_shift=>b_out_p43,
      
      sum=>p43);
      
      
    end behavioral;
      
    
  
    
