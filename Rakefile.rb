DEPS = "ncurses"
SRC = Dir['client/*.c'].join ' '

task :clean do
  Dir['*.o'].each {|f| rm f}
end

task :build => :clean do 
  sh "gcc -c #{SRC}"
  sh "gcc -o nim #{Dir['*.o'].join ' '} #{"-l#{DEPS}" if DEPS.size>0 rescue nil}"
end

task :blank do
  sh "./nim" rescue nil
end

task :new do
  sh "./nim README" rescue nil
end

task :server do
	puts "Starting redis and the nimbus node.js server ... "
	Process.fork { sh "redis-server > redis.log" rescue nil }
	sh "node server/server.js --debug" rescue nil
end

task :console do
  sh "nc 127.0.0.1 8000" rescue puts "Server is offline."
end

task :kill do
  sh "killall nim"
end

task :b => :build
task :r => :blank
task :n => [:build, :new]
task :s => :server
task :c => :console
task :k => :kill
task :default => [:build, :r]