DEPS = ""
SRC = Dir['client/*.c'].join ' '

task :clean do
  Dir['*.o'].each {|f| rm f}
end

task :build => :clean do 
  sh "gcc -c #{SRC}"
  sh "gcc -o nim #{Dir['*.o'].join ' '} #{"-l#{DEPS}" if DEPS.size>0 rescue nil}"
end

task :run do
  sh "./nim" rescue nil
end

task :server do
	puts "Starting redis and the nimbus node.js server ... "
	Process.fork { sh "redis-server > /dev/null" rescue nil }
	sh "node server/server.js --debug" rescue nil
end

task :console do
  sh "nc 127.0.0.1 8000" rescue puts "Server is offline."
end

task :b => :build
task :r => :run
task :s => :server
task :c => :console
task :default => [:build, :run]