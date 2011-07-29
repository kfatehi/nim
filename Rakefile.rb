DEPS = %{}
SRC = Dir['client/*.c'].join ' '

task :clean do
  Dir['*.o'].each {|f| rm f rescue nil}
end

task :b => [:clean] do 
  sh "gcc -c #{SRC}"
  sh "gcc -o nim #{Dir['*.o'].join ' '} #{"-l#{DEPS}" if DEPS.size>0 rescue nil}"
  Dir['*.o'].each {|f| rm f rescue nil}
end

task :br => [:b] do 
  sh "./nim" rescue nil
end

task :r do 
  sh "./nim" rescue nil
end

# r1, r2, r3 or somethin for different ways to exec

task :s do
  sh "node server/server.js --debug"
end

task :c do
  sh "nc 127.0.0.1 8000" rescue puts "Server is offline."
end