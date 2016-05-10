require 'csv'

print "Ola\n"

tickers = {}

#CSV.foreach("VMWareSecurityAdvisoryList.csv", :headers => true, :header_converters => :symbol, :converters => :all) do |row|
CSV.foreach("test.csv", :headers => true, :header_converters => :symbol, :converters => :all) do |row|
  tickers[row.fields[0]] = Hash[row.headers[1..-1].zip(row.fields[1..-1])]
end


print tickers.length," elements\n\n"

tickers.each {|key, value| print "Key=#{key}\n",
                                 "Product=",value[:product], "\n",
                                 "Version=",value[:version], "\n",                             
                                 "Title=",value[:vuln_title], "\n",
                                 "Advisory name=",value[:advisory_name], "\n",
                                 "Description=",value[:vulnerbility_description], "\n\n"
                                 
                                  }